#include <iostream>
#include <memory>
#include <vector>
#include <string>
#include <aff3ct.hpp>

#include "Module/Py_Module/Py_Module.hpp"

#include <pybind11/embed.h>
#include "numpy/arrayobject.h"
#include "pybind11/pybind11.h"
#include "pybind11/numpy.h"
#include "pybind11/stl.h"

namespace py = pybind11;

using namespace aff3ct;
using namespace aff3ct::module;

namespace aff3ct { namespace tools {
using Monitor_BFER_reduction = Monitor_reduction<module::Monitor_BFER<>>;
} }

int main(int argc, char** argv)
{
	// get the AFF3CT version
	const std::string v = "v" + std::to_string(tools::version_major()) + "." +
	                            std::to_string(tools::version_minor()) + "." +
	                            std::to_string(tools::version_release());

	std::cout << "#----------------------------------------------------------"      << std::endl;
	std::cout << "# This is a basic program using the AFF3CT library (" << v << ")" << std::endl;
	std::cout << "# Feel free to improve it as you want to fit your needs."         << std::endl;
	std::cout << "#----------------------------------------------------------"      << std::endl;
	std::cout << "#"                                                                << std::endl;

	size_t n_threads = 1;//std::thread::hardware_concurrency();

	int   K         = 1000;     // number of information bits
	int   N         = 4000;     // codeword size
	int   fe        = 100;     // number of frame errors
	int   seed      =   0;     // PRNG seed for the AWGN channel
	float ebn0_min  =  0.00f; // minimum SNR value
	float ebn0_max  =  10.01f; // maximum SNR value
	float ebn0_step =   1.00f; // SNR step

	float R = (float)K/(float)N; // code rate (R=K/N)

	py::scoped_interpreter guard{}; // start the interpreter and keep it alive
	py::object py_modem = py::module::import("py_modulator").attr("PyModulator")(N);

	// Build the modules
	std::unique_ptr<module::Source_random_fast    <>> source (new module::Source_random_fast    <>(K        ));
	std::unique_ptr<module::Encoder_repetition_sys<>> encoder(new module::Encoder_repetition_sys<>(K, N     ));
	std::unique_ptr<module::Py_Module             <>> modem  (new module::Py_Module             <>(py_modem ));
	std::unique_ptr<module::Channel_AWGN_LLR      <>> channel(new module::Channel_AWGN_LLR      <>(N        ));
	std::unique_ptr<module::Decoder_repetition_std<>> decoder(new module::Decoder_repetition_std<>(K, N     ));
	std::unique_ptr<module::Monitor_BFER          <>> monitor(new module::Monitor_BFER          <>(K, fe    ));

	//   ____________________                ___________________                 ___________________
	//  |  Source::generate  |U_K -----> U_K| Encoder::encode   |X_N -----> X_N1| Modem::modulate   |X_N2
	//   --------------------                -------------------                 -------------------
	// sockets binding (connect the sockets of the tasks = fill the input sockets with the output sockets)
	using namespace module;
	(*encoder)[enc::sck::encode      ::U_K ].bind((*source )[src::sck::generate   ::U_K ]);
	(*modem  )[             "modulate::b"  ].bind((*encoder)[enc::sck::encode     ::X_N ]);
	(*channel)[chn::sck::add_noise   ::X_N ].bind((*modem  )[            "modulate::x"  ]);
	(*decoder)[dec::sck::decode_siho ::Y_N ].bind((*channel)[chn::sck::add_noise  ::Y_N ]);
	(*monitor)[mnt::sck::check_errors::U   ].bind((*encoder)[enc::sck::encode     ::U_K ]);
	(*monitor)[mnt::sck::check_errors::V   ].bind((*decoder)[dec::sck::decode_siho::V_K ]);



	std::unique_ptr<tools::Sequence> sequence(new tools::Sequence((*source)[src::tsk::generate], n_threads));
	std::ofstream f("sequence.dot");
	sequence->export_dot(f);

	std::unique_ptr<tools::Monitor_BFER_reduction> monitor_red(new tools::Monitor_BFER_reduction( sequence->get_modules<module::Monitor_BFER<>>()));
	monitor_red->set_reduce_frequency(std::chrono::milliseconds(500));
	auto tasks_per_types = sequence->get_tasks_per_types();

	// configuration of the sequence tasks
	for (auto& type : tasks_per_types) for (auto& tsk : type)
	{
		tsk->set_autoalloc      (true      ); // enable the automatic allocation of the data in the tasks
		tsk->set_debug          (false     ); // disable the debug mode
		tsk->set_debug_limit    (-1        ); // display only the 8 first bits if the debug mode is enabled
		tsk->set_debug_precision(2         ); // display only all values if the debug mode is enabled
		tsk->set_stats          (true      ); // enable the statistics

		// enable the fast mode (= disable the useless verifs in the tasks) if there is no debug and stats modes
		if (!tsk->is_debug() && !tsk->is_stats())
			tsk->set_fast(true);
	}
	std::unique_ptr<tools::Sigma<>> noise(new tools::Sigma<>());// create a sigma noise type

	// allocate a terminal that will display the collected data from the reporters
	std::vector<std::unique_ptr<tools::Reporter>> reporters;
	// report the noise values (Es/N0 and Eb/N0)
	reporters.push_back(std::unique_ptr<tools::Reporter>(new tools::Reporter_noise<>(*noise, true)));
	// report the bit/frame error rates
	reporters.push_back(std::unique_ptr<tools::Reporter>(new tools::Reporter_BFER<>(*monitor_red)));
	// report the simulation throughputs
	reporters.push_back(std::unique_ptr<tools::Reporter>(new tools::Reporter_throughput<>(*monitor_red)));
	std::unique_ptr<tools::Terminal_std> terminal(new tools::Terminal_std(reporters));

	// set the noise
	for (auto &m : sequence->get_modules<tools::Interface_get_set_noise>())
		m->set_noise(*noise);

	// registering to noise updates
	for (auto &m : sequence->get_modules<tools::Interface_notify_noise_update>())
		noise->record_callback_update([m](){ m->notify_noise_update(); });

	// display the legend in the terminal
	terminal->legend();

	// set different seeds in the modules that uses PRNG
	std::mt19937 prng;
	for (auto &m : sequence->get_modules<tools::Interface_set_seed>())
		m->set_seed(prng());

	// loop over the various SNRs
	for (auto ebn0 = ebn0_min; ebn0 < ebn0_max; ebn0 += ebn0_step)
	{
		// compute the current sigma for the channel noise
		const auto esn0  = tools::ebn0_to_esn0 (ebn0, R);
		const auto sigma = tools::esn0_to_sigma(esn0     );

		noise->set_values(sigma, ebn0, esn0);

		// update the sigma of the modem and the channel
		channel->set_noise(*noise);

		// display the performance (BER and FER) in real time (in a separate thread)
		terminal->start_temp_report();

		py::gil_scoped_release release;

		// run the simulation sequence
		sequence->exec([&monitor_red, &terminal]() { return monitor_red->is_done() || terminal->is_interrupt(); });

		// final reduction
		monitor_red->reduce();

		// display the performance (BER and FER) in the terminal
		terminal->final_report();

		// reset the monitor and the terminal for the next SNR
		monitor_red->reset();
		terminal->reset();

		// if user pressed Ctrl+c twice, exit the SNRs loop
		if (terminal->is_over()) break;
	}

	// display the statistics of the tasks (if enabled)
	std::cout << "#" << std::endl;
	tools::Stats::show(sequence->get_modules_per_types(), true);
	std::cout << "# End of the simulation" << std::endl;

	return 0;
}
