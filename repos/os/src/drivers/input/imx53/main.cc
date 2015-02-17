/**
 * \brief  Input driver front-end
 * \author Norman Feske
 * \author Christian Helmuth
 * \author Stefan Kalkowski
 * \date   2006-08-30
 */

/*
 * Copyright (C) 2006-2013 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU General Public License version 2.
 */

/* Genode includes */
#include <base/env.h>
#include <base/sleep.h>
#include <base/rpc_server.h>
#include <cap_session/connection.h>
#include <platform_session/connection.h>
#include <input/component.h>
#include <input/root.h>
#include <base/printf.h>

/* local includes */
#include <driver.h>

using namespace Genode;

int main(int argc, char **argv)
{
	/* initialize server entry point */
	enum { STACK_SIZE = 4096 };
	static Cap_connection cap;
	static Rpc_entrypoint ep(&cap, STACK_SIZE, "input_ep");

	static Input::Session_component session;

	Platform::Connection plat_drv;
	switch (plat_drv.revision()) {
	case Platform::Session::SMD:
		plat_drv.enable(Platform::Session::I2C_2);
		plat_drv.enable(Platform::Session::I2C_3);
		plat_drv.enable(Platform::Session::BUTTONS);
		Input::Tablet_driver::factory(session.event_queue());
		break;
	default:
		PWRN("No input driver available for this board");
	}

	/* entry point serving input root interface */
	static Input::Root_component root(ep, session);

	/* tell parent about the service */
	env()->parent()->announce(ep.manage(&root));

	/* main's done - go to sleep */
	sleep_forever();
	return 0;
}
