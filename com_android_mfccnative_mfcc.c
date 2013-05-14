//#include "math.h"
//#include "stdio.h"
#include "com_android_mfccnative_MFCCLib.h"
#include "codegen/lib/fi_mfcc/mfcc_bare.h"


void castReal(JNIEnv *env, jdoubleArray *input, int begin, int end, real_T *out){
	jsize length = begin - end;
	jdouble element[1];
	jsize i;
	for (i = 0; 0 < length; ++i){
		(*env)->GetDoubleArrayRegion(env, input, begin+i, 1, element);
		out[i] = element[0];
	}
}

void castComplex(JNIEnv *env, jdoubleArray *input, int begin, int end, creal_T *out){
	jsize length = begin - end;
	jdouble element[2];
	jsize i;
	for (i = 0; 0 < length; ++i){
		(*env)->GetDoubleArrayRegion(env, input, begin+i*2, 2, element);
		out[i].re = element[0];
		out[i].im = element[1];
	}
}

void realToDoubleCopy(int idx, int frameSize, creal_T *in, double *out){
	int i;
	for (i = 0; i < frameSize; ++i){
		out[idx*frameSize + i] = in[i].re;
	}
}

JNIEXPORT jdoubleArray JNICALL Java_com_android_mfccnative_MFCCLib_mfcc (JNIEnv *env, jclass clazz, jdoubleArray frames, jdoubleArray hamming, jdoubleArray mel_filters, jdoubleArray dct_coeff){
	//__android_log_print(ANDROID_LOG_VERBOSE, APPNAME, "Debug: I am now debugging JNI Code %d", 1+1);
	//Prealocate memory for audio data
	int frameSize = 128;
	int numFrames = (int) ceil((double)(*env)->GetArrayLength(env, frames) / (double) frameSize);
	int numFrameElements = frameSize * numFrames;
	real_T d_frame[128];

	//Setup hamming data
	real_T d_hamming[128];
	castReal(env, hamming, 0, frameSize, d_hamming);

	//Setup mel filter data
	real_T d_mel_filters[2016];
	castReal(env, mel_filters, 0, 2016, d_mel_filters);

	//Setup dct data
	creal_T d_dct_coeff[32];
	castComplex(env,dct_coeff, 0, 32, d_dct_coeff);

	creal_T tmpMel[13];
	int numFeatureElements = 13 * numFrames;

	double *d_mel = malloc(numFeatureElements * sizeof *d_mel);
	real_T fftA = 2.0;
	real_T fftB = 64.0;

	//Timer Start
	int i;
	for (i = 0; i < numFrames; ++i){
		castReal(env, frames, i*frameSize, i*frameSize + frameSize, d_frame);
		mfcc_bare(d_frame, d_hamming, d_mel_filters, fftA, fftB, d_dct_coeff, tmpMel);
		//realToDoubleCopy(i, frameSize, tmpMel, d_mel);

	}

	//Timer End
	jdoubleArray j_mel = (*env)->NewDoubleArray(env,numFeatureElements);
	(*env)->SetDoubleArrayRegion(env, j_mel, 0, numFeatureElements, (const jdouble*) d_mel);

	// (*env)->SetDoubleArrayRegion( env, doubleArray, 0, 16, (const jdouble*) gl_para );
	//Delete all dynamic allocations
	free((void*) d_mel);
	return j_mel;
}

