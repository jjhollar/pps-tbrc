#ifndef FileConstants_h
#define FileConstants_h

#include <stdint.h>
#include <stdlib.h>
#include <string>

#include "VME_TDCEvent.h"

/**
 * General header to store in each collected data file for offline readout. It
 * enable any reader to retrieve the run/spill number, as well as the HPTDC
 * configuration during data collection.
 * \brief Header to the output files
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 14 Apr 2015 
 */
struct file_header_t {
  uint32_t magic;
  uint32_t run_id;
  uint32_t spill_id;
  uint8_t num_hptdc;
  VME::AcquisitionMode acq_mode;
  VME::DetectionMode det_mode;
};

/// Generate a random file name
inline std::string GenerateFileName(const size_t len=5)
{
  std::string out;
  srand(time(NULL));
  const char az[] =
    "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";
  out = "events_";
  for (size_t i=0; i<len; i++) { out += az[rand()%(sizeof(az)-1)]; }
  out += ".dat";
  return out;
}

#endif
