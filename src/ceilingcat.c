#include <AR/video.h>
#include <AR/param.h>
#include <AR/ar.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

const char* cparam_name = "Data/camera_para.dat";

const char* PATTERN_NAMES[] = {
	"Data/patt.hiro", "Data/patt.kanji"
};

#define DIE_HORRIBLY() { printf("HORRIBLE DEATH CAKES: %d\n", __LINE__); abort(); }

#define NPATTERNS ( sizeof(PATTERN_NAMES) / sizeof(*PATTERN_NAMES) )

const char* VCONF = "-width=640 -height=480 -nodialog";

#define NSAMPLES 16
#define NOUTLIERS 5

float distanceSamples[NSAMPLES*NPATTERNS];
float angleSamples[NSAMPLES*NPATTERNS];

int sampleIndex = 0;

float outlierExcludedAverage(const float* array, int len, int outliers)
{
	int i, j;
	bool done;
	float value;
	float* local = alloca(len*sizeof(float));
	memcpy(local, array, len*sizeof(float));
	for (i = 0; i < len - 1; ++i)
	{
		value = local[i];
		j = i - 1;
		done = false;
		do
		{
			local[j + 1] = local[j];
			--j;
			if (j < 0)
				done = true;
		}
		while (!done);
		local[j + 1] = value;
	}
	value = 0.0f;
	for (i = outliers; i < len - outliers; ++i)
	{
		value += local[i];
	}
	value /= (float)(len - 2*outliers);
	return value;
}

int main()
{
	// declare some stuff
	bool seen, ellipsis = false;
	ARParam wparam, cparam;
	int xsize = 640, ysize = 480;
	int patterns[NPATTERNS];
	int thresh        = 100;

	// declare some more stuff
	ARUint8* data;
	ARMarkerInfo* marker_info;
	int marker_num;
	int j, k;

	// even more stuff
	const double patt_width = 80.0;
	double patt_centre[2]   = {
		0.0, 0.0
	};
	double patt_trans[3][4];
	double cam_trans[3][4];

	// open the video feed
	if (arVideoOpen( (char*)VCONF ) < 0)
		DIE_HORRIBLY();

	// set x and y size
	if (arVideoInqSize(&xsize, &ysize) < 0)
		DIE_HORRIBLY();

	// load the camera parameters
	// this is necessary to account for things like barrel distortion
	if (arParamLoad(cparam_name, 1, &wparam) < 0)
		DIE_HORRIBLY();
	// do some more shit
	arParamChangeSize(&wparam, xsize, ysize, &cparam);

	// and, uh
	// actually i have no idea what this does
	arInitCparam(&cparam);
	arParamDisp(&cparam);

	// START THE CAPTURE
	arVideoCapStart();

	// load the pattern... in the loop!
	for (j = 0; j < NPATTERNS; ++j)
	{
		if (patterns[j] = arLoadPatt(PATTERN_NAMES[j]) < 0)
			DIE_HORRIBLY();
		printf("Loaded pattern %s as %d\n", PATTERN_NAMES[j], patterns[j]);
	}

	// OH NOES, COULD IT BE AN INFINITE LOOP?!
	while (1)
	{
		// get a still from the video
		// if it bites you, let it go
		// (eeny meeny miny mo)
		if ( ( data = (ARUint8*)arVideoGetImage() ) == NULL )
		{
			usleep(100000);
			continue;
		}

		// look for the markers
		if (arDetectMarker(data, thresh, &marker_info, &marker_num) < 0)
			DIE_HORRIBLY();

		// advance to the next still
		arVideoCapNext();

		seen = false;
		// loop over the loaded markers, working out which ones we've seen
		for (j = 0; j < marker_num; ++j)
		{
			for (k = 0; k < NPATTERNS; ++k)
			{
				if (patterns[k] == marker_info[j].id)
				{
					if (!seen)
					{
						printf("%c[2J", 27);
						seen = true;
						ellipsis = false;
					}
					arGetTransMat(&marker_info[k],
					              patt_centre,
					              patt_width,
					              patt_trans);
					arUtilMatInv(patt_trans, cam_trans);

					float distance = sqrtf(cam_trans[0][3]*cam_trans[0][3]
					                       +cam_trans[1][3]*cam_trans[1][3]
					                       +cam_trans[2][3]*cam_trans[2][3]);
					float angle =
					    acosf(cam_trans[0][3] / distance) - (M_PI / 2.0f);

					distance /= 800.0f;
					angle *= (180.0f / M_PI);

					printf( "saw %d    d: %2.3f m   a: %2.1f\n",
					         k,
					         distance,
					         angle );

					distanceSamples[(k*NSAMPLES)+sampleIndex] = distance;
					angleSamples[(k*NSAMPLES)+sampleIndex] = angle;

					printf( " dejitter d: %2.3f m   a: %2.1f\n",
					         outlierExcludedAverage(distanceSamples + (k*NSAMPLES), NSAMPLES, NOUTLIERS),
					         outlierExcludedAverage(angleSamples + (k*NSAMPLES), NSAMPLES, NOUTLIERS ) );
				}
			}
		}
		sampleIndex = (sampleIndex + 1) % NSAMPLES;
		if (!seen && !ellipsis)
		{
			printf("...\n");
			ellipsis = true;
		}
		fflush(stdout);
		// exit, pursued by a bear
	}
}

