/*
 * This file is part of Xwing - Open video hosting engine
 *
 * Copyright (C) 2014 - Xpast; http://xpast.me/; <vvladxx@gmail.com>
 *
 * Xwing is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * Xwing is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Xwing.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "common.hpp"
#include "js.hpp"

namespace JS {

JSRuntime * MainInstance::rt = 0;
JSContext * MainInstance::cx = 0;
const size_t MainInstance::stack_chunk_size = 8192;

void MainInstance::error_reporter (JSContext *, const char * message, JSErrorReport * report) throw() {
	if (JSREPORT_IS_WARNING(report->flags)) {
		if (report->filename) {
			DEBUG_PRINT_1(report->filename << ':' << report->lineno << ": --> " << report->tokenptr << message);
		}
		else if (report->tokenptr) {
			DEBUG_PRINT_1(" --> " << report->tokenptr << message);
		}
		else {
			DEBUG_PRINT_1(message);
		}
	}
	else {
		if (report->filename) {
			LOG_ERROR(report->filename << ':' << report->lineno << ": --> " << report->tokenptr << message);
		}
		else if (report->tokenptr) {
			LOG_ERROR(" --> " << report->tokenptr << message);
		}
		else {
			LOG_ERROR(message);
		}
	}
}

void MainInstance::init () {
	if (!JS_Init())
		LOG_CRITICAL("JS_Init() failed"_tr);
	rt = JS_NewRuntime(DefaultHeapMaxBytes);
	if (!rt)
		LOG_CRITICAL("JS_NewRuntime() failed"_tr);
	cx = JS_NewContext(rt, stack_chunk_size);
	if (!cx)
		LOG_CRITICAL("JS_NewContext() failed"_tr);
	JS_SetErrorReporter(cx, error_reporter);
	process_before_exit(JS::MainInstance::exit);
}

JSObject * MainInstance::new_global () {
	static JSClass globalClass = {
		"global",
		JSCLASS_GLOBAL_FLAGS,
		JS_PropertyStub,
		JS_DeletePropertyStub,
		JS_PropertyStub,
		JS_StrictPropertyStub,
		JS_EnumerateStub,
		JS_ResolveStub,
		JS_ConvertStub,
		nullptr, nullptr, nullptr, nullptr,
		JS_GlobalObjectTraceHook
	};
	return JS_NewGlobalObject(cx, &globalClass, nullptr, FireOnNewGlobalHook);
}

bool MainInstance::run_file (HandleObject global, const char * filepath) {
	CompileOptions options(cx, JSVERSION_LATEST);
	options.setUTF8(true);
	JSAutoRequest ar(cx);
	JSAutoCompartment ac(cx, global);
	return JS::Evaluate(cx, global, options, filepath);
}

void MainInstance::exit () {
	if (!cx)
		return;
	JS_DestroyContext(cx);
	JS_DestroyRuntime(rt);
	cx = 0, rt = 0;
	JS_ShutDown();
}

};
