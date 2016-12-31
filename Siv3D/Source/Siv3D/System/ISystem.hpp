﻿//-----------------------------------------------
//
//	This file is part of the Siv3D Engine.
//
//	Copyright (C) 2008-2017 Ryo Suzuki
//	Copyright (C) 2016-2017 OpenSiv3D Project
//
//	Licensed under the MIT License.
//
//-----------------------------------------------

# pragma once
# include <Siv3D/Fwd.hpp>

namespace s3d
{
	class ISiv3DSystem
	{
	public:

		static ISiv3DSystem* Create();

		virtual ~ISiv3DSystem() = default;

		virtual bool init() = 0;

		virtual void exit() = 0;

		virtual bool update() = 0;

		virtual void reportEvent(uint32 windowEventFlag) = 0;
	};
}
