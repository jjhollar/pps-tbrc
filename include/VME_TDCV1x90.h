#ifndef VMETDCV1x90_H 
#define VMETDCV1x90_H

//#define DEBUG
#include <iostream>
#include <iomanip>
#include <sstream>

#include <cmath>
#include <string>
#include <map>

#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include "CAENVMElib.h"
#include "CAENVMEoslib.h"
#include "CAENVMEtypes.h"

#include "VME_TDCV1x90Opcodes.h"
#include "VME_TDCEvent.h"
#include "Exception.h"

#define TDC_ACQ_STOP 20000

namespace VME
{
  typedef enum {
    MATCH_WIN_WIDTH        = 0,
    WIN_OFFSET             = 1,
    EXTRA_SEARCH_WIN_WIDTH = 2,
    REJECT_MARGIN          = 3,
    TRIG_TIME_SUB          = 4,
  } trig_conf;

  typedef enum {
    r800ps = 0,
    r200ps = 1,
    r100ps = 2,
    r25ps  = 3,
  } trailead_edge_lsb;

  typedef enum {
    WRITE_OK      = 0, /*!< \brief Is the TDC ready for writing? */
    READ_OK       = 1, /*!< \brief Is the TDC ready for reading? */
  } micro_handshake;

  typedef enum {
    CONT_STORAGE,
    TRIG_MATCH,
  } acq_mode;

  typedef enum {
    PAIR      = 0,
    OTRAILING = 1,
    OLEADING  = 2,
    TRAILEAD  = 3,
  } det_mode;

  typedef enum {

    kOutputBuffer            = 0x0000, // D32 R
    kControl                 = 0x1000, // D16 R/W
    kStatus                  = 0x1002, // D16 R
    kInterruptLevel          = 0x100a, // D16 R/W
    kInterruptVector         = 0x100c, // D16 R/W
    kGeoAddress              = 0x100e, // D16 R/W
    kMCSTBase                = 0x1010, // D16 R/W
    kMCSTControl             = 0x1012, // D16 R/W
    kModuleReset             = 0x1014, // D16 W
    kSoftwareClear           = 0x1016, // D16 W
    kEventCounter            = 0x101c, // D32 R
    kEventStored             = 0x1020, // D16 R
    kBLTEventNumber          = 0x1024, // D16 R/W
    kFirmwareRev             = 0x1026, // D16 R
    kMicro                   = 0x102e, // D16 R/W
    kMicroHandshake          = 0x1030, // D16 R
    
    kEventFIFO               = 0x1038, // D32 R
    kEventFIFOStoredRegister = 0x103c, // D16 R
    kEventFIFOStatusRegister = 0x103e, // D16 R  
    
    kROMOui2                 = 0x4024,
    kROMOui1                 = 0x4028,
    kROMOui0                 = 0x402c,
    
    kROMBoard2               = 0x4034,
    kROMBoard1               = 0x4038,
    kROMBoard0               = 0x403c,
    kROMRevis3               = 0x4040,
    kROMRevis2               = 0x4044,
    kROMRevis1               = 0x4048,
    kROMRevis0               = 0x404c,
    kROMSerNum1              = 0x4080,
    kROMSerNum0              = 0x4084,
    
  } mod_reg;


  struct GlobalOffset {
    uint16_t coarse;
    uint16_t fine;
  };

  // Event structure (one for each trigger)

  struct trailead_t {
    uint32_t event_count;
    int total_hits[16];
    // key   -> channel,
    // value -> measurement
    std::multimap<int32_t,int32_t> leading;
    std::multimap<int32_t,int32_t> trailing;
    uint32_t ettt;
  };

  /**
   * \brief TDC status register
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date Jun 2015
   */
  class TDCV1x90Status
  {
    public:
      typedef enum {
        R_800ps = 0x0,
        R_200ps = 0x1,
        R_100ps = 0x2,
        R_25ps = 0x3
      } TDCResolution;

      inline TDCV1x90Status(const uint16_t& word) : fWord(word) {;}
      virtual inline ~TDCV1x90Status() {;}

      inline void Dump() const {
        std::ostringstream ss;
        ss << "============ TDC Status ============" << "\n\t"
           << "----- FIFO state: ---------------" << "\n\t"
           << "Data ready? " << DataReady() << "\n\t"
           << "FIFO almost full? " << AlmostFull() << "\n\t"
           << "FIFO full? " << Full() << "\n\t"
           << "FIFO purged? " << Purged() << "\n\t"
           << "---------------------------------" << "\n\t"
           << "Trigger matching mode? " << TriggerMatching() << "\n\t"
           << "Pair mode? " << PairMode() << "\n\t"
           << "Resolution: " << Resolution() << "\n\t"
           << "Headers enabled? " << HeadersEnabled() << "\n\t"
           << "Termination? " << TerminationOn() << "\n\t"
           << "----- Error state: --------------" << "\n\t"
           << "Global error: " << Error() << "\n\t";
        for (unsigned int i=0; i<4; i++) ss << "\tError " << i << ": " << Error(i) << "\n\t";
        ss << "Bus error: " << BusError() << "\n\t"
           << "---------------------------------" << "\n\t"
           << "Trigger lost? " << TriggerLost();
        PrintInfo(ss.str());
      }
 
      inline uint16_t GetValue() const { return fWord; }
      
      inline bool DataReady() const { return static_cast<bool>(fWord&0x1); }
      inline bool AlmostFull() const { return static_cast<bool>((fWord>>1)&0x1); }
      inline bool Full() const { return static_cast<bool>((fWord>>2)&0x1); }
      inline bool TriggerMatching() const { return static_cast<bool>((fWord>>3)&0x1); }
      inline bool HeadersEnabled() const { return static_cast<bool>((fWord>>4)&0x1); }
      inline bool TerminationOn() const { return static_cast<bool>((fWord>>5)&0x1); }
      inline bool Error(const unsigned int& id) const { return static_cast<bool>((fWord>>(6+id))&0x1); }
      inline bool Error() const { return (Error(0) or Error(1) or Error(2) or Error(3)); }
      inline bool BusError() const { return static_cast<bool>((fWord>>10)&0x1); }
      inline bool Purged() const { return static_cast<bool>((fWord>>11)&0x1); }
      inline TDCResolution Resolution() const { return static_cast<TDCResolution>((fWord>>12)&0x3); }
      inline bool PairMode() const { return static_cast<bool>((fWord>>14)&0x1); }
      inline bool TriggerLost() const { return static_cast<bool>((fWord>>15)&0x1); }
      
    private:
      uint16_t fWord;
  };

  /**
   * \brief TDC control register
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \date Jun 2015
   */
  class TDCV1x90Control
  {
    public:
      inline TDCV1x90Control(const uint16_t& word) : fWord(word) {;}
      inline virtual ~TDCV1x90Control() {;}

      inline void Dump() const {
        std::ostringstream ss;
        ss << "============ TDC Control ============\n\t"
           << "Bus error? " << GetBusError() << "\n\t"
           << "Termination? " << GetTermination() << "\n\t"
           << "Termination (SW)? " << GetSWTermination() << "\n\t"
           << "Empty events? " << GetEmptyEvent() << "\n\t"
           << "Align word sizes to even number? " << GetAlign64() << "\n\t"
           << "Compensation? " << GetCompensation() << "\n\t"
           << "FIFO test? " << GetTestFIFO()  << "\n\t"
           << "Compensation (SRAM)? " << GetSRAMCompensation() << "\n\t"
           << "Event FIFO? " << GetEventFIFO() << "\n\t"
           << "ETTT? " << GetETTT() << "\n\t"
           << "MEB access with 16MB address range? " << GetMEBAccess();
        PrintInfo(ss.str());
      }

      inline uint16_t GetValue() const { return fWord; }
      
      inline bool GetBusError() const { return static_cast<bool>(fWord&0x1); }
      inline void SetBusError(bool sw) { if (sw==GetBusError()) return; int sign = (sw)?1:-1; fWord+=sign; }

      inline bool GetTermination() const { return static_cast<bool>((fWord>>1)&0x1); }
      inline void SetTermination(bool sw) { if (sw==GetTermination()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<1); }

      inline bool GetSWTermination() const { return static_cast<bool>((fWord>>2)&0x1); }
      inline void SetSWTermination(bool sw) { if (sw==GetSWTermination()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<2); }

      inline bool GetEmptyEvent() const { return static_cast<bool>((fWord>>3)&0x1); }
      inline void SetEmptyEvent(bool sw) { if (sw==GetEmptyEvent()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<3); }

      inline bool GetAlign64() const { return static_cast<bool>((fWord>>4)&0x1); }
      inline void SetAlign64(bool sw) { if (sw==GetAlign64()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<4); }

      inline bool GetCompensation() const { return static_cast<bool>((fWord>>5)&0x1); }
      inline void SetCompensation(bool sw) { if (sw==GetCompensation()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<5); }

      inline bool GetTestFIFO() const { return static_cast<bool>((fWord>>6)&0x1); }
      inline void SetTestFIFO(bool sw) { if (sw==GetTestFIFO()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<6); }

      inline bool GetSRAMCompensation() const { return static_cast<bool>((fWord>>7)&0x1); }
      inline void SetSRAMCompensation(bool sw) { if (sw==GetSRAMCompensation()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<7); }

      inline bool GetEventFIFO() const { return static_cast<bool>((fWord>>8)&0x1); }
      inline void SetEventFIFO(bool sw) { if (sw==GetEventFIFO()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<8); }

      inline bool GetETTT() const { return static_cast<bool>((fWord>>9)&0x1); }
      inline void SetETTT(bool sw) { if (sw==GetETTT()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<9); }

      inline bool GetMEBAccess() const { return static_cast<bool>((fWord>>12)&0x1); }
      inline void SetMEBAccess(bool sw) { if (sw==GetMEBAccess()) return; int sign = (sw)?1:-1; fWord+=(sign*0x1<<12); }

    private:
      uint16_t fWord;
  };

  /**
   * 
   * \author Laurent Forthomme <laurent.forthomme@cern.ch>
   * \author Bob Velghe <bob.velghe@cern.ch>
   * \date Jun 2010 (NA62-Gigatracker)
   * \date May 2015 (CMS-TOTEM PPS)
   */
  class TDCV1x90
  {
    public:
      TDCV1x90(int32_t, uint32_t, acq_mode acqm=TRIG_MATCH, det_mode detm=TRAILEAD);
      ~TDCV1x90();
      void SetVerboseLevel(unsigned short verb=1) { fVerb=verb; }

      void SetTestMode(bool en=true) const;
      bool GetTestMode() const;
      
      uint32_t GetModel() const;
      uint32_t GetOUI() const;
      uint32_t GetSerialNumber() const;
      void GetFirmwareRev() const;
      
      void CheckConfiguration() const;
     
      void EnableChannel(short) const;
      void DisableChannel(short) const;
      void SetPoI(uint16_t);

      void SetLSBTraileadEdge(trailead_edge_lsb) const;

      void SetAcquisitionMode(acq_mode);
      void ReadAcquisitionMode();
      inline acq_mode GetAcquisitionMode() {
        ReadAcquisitionMode();
        return fAcquisitionMode;
      }
      void SetTriggerMatching();
      void SetContinuousStorage();

      void SetDetection(det_mode);
      void ReadDetection();
      inline det_mode GetDetection() {
        ReadDetection();
        return fDetectionMode;
      }
      
      void SetGlobalOffset(const GlobalOffset&) const;
      GlobalOffset GetGlobalOffset() const;
      
      void SetRCAdjust(int,uint16_t) const;
      uint16_t GetRCAdjust(int) const;
      
      /**
       * Number of acquired events since the latest module’s reset/clear;
       * this counter works in trigger Matching Mode only.
       * \brief Number of occured triggers
       */
      uint32_t GetEventCounter() const;
      /**
       * \brief Number of events currently stored in the output buffer
       */
      uint16_t GetEventStored() const;
      
      void SetTDCEncapsulation(bool) const;
      bool GetTDCEncapsulation() const;

      void SetErrorMarks(bool mode=true);
      inline bool GetErrorMarks() const { return fErrorMarks; }
      
      void SetPairModeResolution(int,int) const;
      uint16_t GetResolution(const det_mode&) const;

      void SetBLTEventNumberRegister(const uint16_t&) const;
      uint16_t GetBLTEventNumberRegister() const;
      
      void SetWindowWidth(const uint16_t&);
      inline uint16_t GetWindowWidth() const { return fWindowWidth; }

      void SetWindowOffset(const int16_t&) const;
      int16_t GetWindowOffset() const;

      uint16_t GetTriggerConfiguration(const trig_conf&) const;

      bool WaitMicro(micro_handshake) const;
      bool SoftwareClear() const;
      bool SoftwareReset() const;
      bool HardwareReset() const;
      
      inline void SetETTT(bool ettt=true) const {
        TDCV1x90Control ctl = GetControl();
        ctl.SetETTT(ettt);
        SetControl(ctl);
      }
      inline bool GetETTT() const { return GetControl().GetETTT(); }
      
      void SetStatus(const TDCV1x90Status&) const;
      TDCV1x90Status GetStatus() const;

      void SetControl(const TDCV1x90Control&) const;
      TDCV1x90Control GetControl() const;

      TDCEventCollection FetchEvents();

      //bool IsEventFIFOReady();
      void SetFIFOSize(const uint16_t&) const;
      uint16_t GetFIFOSize() const;
      
      // Close/Clean everything before exit
      void abort();
      
    private:
      /**
       * Write a 16-bit word in the register
       * \brief Write on register
       * \param[in] addr register
       * \param[in] data word
       */
      void WriteRegister(mod_reg, const uint16_t&) const;
      /**
       * Write a 32-bit word in the register
       * \brief Write on register
       * \param[in] addr register
       * \param[in] data word
       */
      void WriteRegister(mod_reg, const uint32_t&) const;
      /**
       * Read a 16-bit word in the register
       * \brief Read on register
       * \param[in] addr register
       * \param[out] data word
       */  
      void ReadRegister(mod_reg, uint16_t*) const;
      /**
       * Read a 32-bit word in the register
       * \brief Read on register
       * \param[in] addr register
       * \param[out] data word
       */  
      void ReadRegister(mod_reg, uint32_t*) const;

      uint32_t fBaseAddr;
      int32_t fHandle;
      unsigned short fVerb;

      acq_mode fAcquisitionMode;
      det_mode fDetectionMode;

      bool fErrorMarks;
      uint16_t fWindowWidth;
      
      CVAddressModifier am; // Address modifier
      CVAddressModifier am_blt; // Address modifier (Block Transfert)
      
      uint32_t* fBuffer;
        
      uint32_t nchannels;
      bool gEnd;
      std::string pair_lead_res[8]; 
      std::string pair_width_res[16];
      std::string trailead_edge_res[4];

  };
}

#endif

