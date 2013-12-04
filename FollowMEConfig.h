#pragma once
/*
this function stores the necessary configuration of the program
in the future release, we will provide the functionality of loading parameter for a local file
*/
class FollowMEConfig
{
public:
	FollowMEConfig(void);
	~FollowMEConfig(void);
public:
	// the directory for storing the image
	// not used
	char snap_shoot_dir[255];
	// the length of sequence
	// to make the algorithm robust, we classify each image in the sequence
	int sequence_length;
	// the threshold for a gesture
	// if the number of positives is larger than the threshold, we mark the gesture detected
	int gesture_threshold;
	// the interval between the detection and tracking, for every "detection_interval" frames we performan
	// a detection to correct the model, for the other frames, we will simple do tracking (with fixed size window)
	int detection_interval;
	// the threshold for defining active optical flow
	// we currently use the sum of absolute differences between two adjacent frames
	double diff_threshold;
};
