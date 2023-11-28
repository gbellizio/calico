﻿#include "utils.h"
#include "agents/image_resizer.h"
#include "agents/image_viewer_live.h"
#include "agents/remote_control.h"
#include "agents/face_detector.h"
#include "agents/error_logger.h"
#include "producers/image_producer_recursive.h"

int main()
{
	const auto ctrl_c = calico::utils::get_ctrlc_token();

	const so_5::wrapped_env_t sobjectizer;
	const auto main_channel = sobjectizer.environment().create_mbox("main");
	auto commands_channel = sobjectizer.environment().create_mbox("commands");
	auto resized_images = sobjectizer.environment().create_mbox("resized");

	sobjectizer.environment().introduce_coop(so_5::disp::active_obj::make_dispatcher(sobjectizer.environment()).binder(), [&](so_5::coop_t& c) {
		c.make_agent<calico::producers::image_producer_recursive>(main_channel, commands_channel);
		c.make_agent<calico::agents::image_resizer>(main_channel, resized_images, 0.5);
		c.make_agent<calico::agents::error_logger>(main_channel);
		c.make_agent<calico::agents::image_viewer_live>(
			c.make_agent<calico::agents::face_detector>(resized_images)->output());
		c.make_agent<calico::agents::remote_control>(commands_channel);
	});

	calico::utils::wait_for_stop(ctrl_c);
}
