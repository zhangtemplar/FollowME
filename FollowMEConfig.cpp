#include "StdAfx.h"
#include "FollowMEConfig.h"

FollowMEConfig::FollowMEConfig(void)
{
	sequence_length=10;
	gesture_threshold=3;
	diff_threshold=1351680;
	detection_interval=5;
}

FollowMEConfig::~FollowMEConfig(void)
{
}
