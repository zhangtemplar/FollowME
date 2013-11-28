#include "StdAfx.h"
#include "FollowMEConfig.h"

FollowMEConfig::FollowMEConfig(void)
{
	sequence_length=4;
	gesture_threshold=2;
	diff_threshold=100000;
	detection_interval=5;
}

FollowMEConfig::~FollowMEConfig(void)
{
}
