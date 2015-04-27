#ifndef FPGAHandler_h
#define FPGAHandler_h

#include "Client.h"
#include "USBHandler.h"
#include "TDC.h"

#define NUM_HPTDC 4 // number of HPTDC per module

#include <fstream>
/**
 * \defgroup FPGA FPGA board control
 */

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
  TDCSetup config[NUM_HPTDC];
};

/**
 * Main driver for a homebrew FPGA designed for the timing detectors' HPTDC
 * chip readout.
 * \brief Driver for timing detectors' FPGA readout
 * \author Laurent Forthomme <laurent.forthomme@cern.ch>
 * \date 14 Apr 2015
 * \ingroup FPGA
 */
class FPGAHandler : public Client, private USBHandler
{
  public:
    /// Bind to a FPGA through the USB protocol, and to the socket
    FPGAHandler(int port, const char* dev);
    virtual ~FPGAHandler();
    
    /// Open an output file to store header/HPTDC events
    void OpenFile();
    /// Close a previously opened output file used to store header/HPTDC events
    void CloseFile();
    /// Retrieve the file name used to store data collected from the FPGA
    inline std::string GetFilename() const { return fFilename; }
    
    inline TDC* GetTDC(unsigned int i=0) {
      if (i<0 or i>=NUM_HPTDC) return 0;
      return fTDC[i];
    }
    inline void SetTDCSetup(const TDCSetup& s) {
      for (unsigned int i=0; i<NUM_HPTDC; i++) {
        fTDC[i]->SetSetupRegister(s);
      }
    }
    
    bool ErrorState();
    
    void ReadBuffer();
    /// Socket actor type retrieval method
    inline SocketType GetType() const { return DETECTOR; }

  private:    
    std::string fFilename;
    std::ofstream fOutput;
    bool fIsFileOpen;
    
    TDC* fTDC[NUM_HPTDC];
    bool fIsTDCInReadout;
};

#endif
