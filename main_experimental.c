/* This file was written by Bill Cox in 2010, and is licensed under the Apache
   2.0 license.

   This file is meant as a simple example for how to use libsonic.  It is also a
   useful utility on its own, which can speed up or slow down wav files, change
   pitch, and scale volume. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sonic_experimental.h"
#include "wave.h"

/* temp */
static int counter = 0;

/* Run the experimental version of sonic. */
static void runSonic(char* inFileName, char* outFileName, float speed) {
  waveFile inFile, outFile = NULL;
  short inBuffer[SONIC_INPUT_SAMPLES], outBuffer[SONIC_INPUT_SAMPLES];
  int samplesRead, samplesWritten, sampleRate, numChannels;

  inFile = openInputWaveFile(inFileName, &sampleRate, &numChannels);
  if (numChannels != 1) {
    fprintf(stderr, "sonic_experimental only processes mono wave files.  This file has %d channels.\n",
        numChannels);
    exit(1);
  }
  if (inFile == NULL) {
    fprintf(stderr, "Unable to read wave file %s\n", inFileName);
    exit(1);
  }
  printf("Sample rate %d\n", sampleRate);
  outFile = openOutputWaveFile(outFileName, sampleRate, 1);
  if (outFile == NULL) {
    closeWaveFile(inFile);
    fprintf(stderr, "Unable to open wave file %s for writing\n", outFileName);
    exit(1);
  }
  sonicInit(speed, sampleRate);
  do {
    samplesRead = readFromWaveFile(inFile, inBuffer, SONIC_INPUT_SAMPLES);
    if (samplesRead == 0) {
      sonicFlushStream();
    } else {
      sonicWriteShortToStream(inBuffer, samplesRead);
    }
    do {
      samplesWritten = sonicReadShortFromStream(outBuffer, SONIC_INPUT_SAMPLES);
      if (samplesWritten > 0) {
        writeToWaveFile(outFile, outBuffer, samplesWritten);
      }
  /* temp */
  counter++;
    } while (samplesWritten > 0);
  } while (samplesRead > 0);
  closeWaveFile(inFile);
  closeWaveFile(outFile);
}

/* Print the usage. */
static void usage(void) {
  fprintf(
      stderr,
      "Usage: sonic_experimental [OPTION]... infile outfile\n"
      "    -s speed   -- Set speed up factor.  2.0 means 2X faster.\n");
  exit(1);
}

int main(int argc, char** argv) {
  char* inFileName;
  char* outFileName;
  float speed = 1.0f;
  int xArg = 1;

  while (xArg < argc && *(argv[xArg]) == '-') {
    if (!strcmp(argv[xArg], "-s")) {
      xArg++;
      if (xArg < argc) {
        speed = atof(argv[xArg]);
        printf("Setting speed to %0.2fX\n", speed);
      }
    }
    xArg++;
  }
  if (argc - xArg != 2) {
    usage();
  }
  inFileName = argv[xArg];
  outFileName = argv[xArg + 1];
  runSonic(inFileName, outFileName, speed);
  return 0;
}
