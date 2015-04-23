#include "TDCSetup.h"

TDCSetup::TDCSetup()
{
  for (uint8_t i=0; i<sizeof(fWord)/sizeof(fWord[0]); i++) {
    //fWord[i] = (1<<WORD_SIZE)-1;
    fWord[i] = 0;
  }
  SetConstantValues();
}

TDCSetup::TDCSetup(const TDCSetup& c)
{
  for (uint8_t i=0; i<sizeof(fWord)/sizeof(fWord[0]); i++) {
    fWord[i] = c.fWord[i];
  }
  SetConstantValues();
}

void
TDCSetup::SetBits(uint16_t lsb, uint16_t word, uint8_t size)
{
  if (size<=0 or size>16) return;
  //FIXME FIXME FIXME burp...
  // See http://www.ioccc.org/ for more information
  for (uint8_t i=0; i<size; i++) {
    uint16_t bit = lsb+i;
    uint8_t bit_rel = bit % WORD_SIZE;
    uint8_t word_id = (bit-bit_rel)/WORD_SIZE;
    fWord[word_id] &=~(1<<bit_rel); // first we clear the bit
    fWord[word_id] |= (((word>>i)&0x1)<<bit_rel); // then we set it
  }
}

uint16_t
TDCSetup::GetBits(uint16_t lsb, uint8_t size) const
{
  if (size<=0 or size>16) return -1;
  uint16_t out = 0x0;
  for (uint8_t i=0; i<size; i++) {
    uint16_t bit = lsb+i;
    uint8_t bit_rel = bit % WORD_SIZE;
    uint8_t word_id = (bit-bit_rel)/WORD_SIZE;
    out |= (((fWord[word_id]>>bit_rel)&0x1)<<i);
  }
  return out;
}

void
TDCSetup::SetConstantValues()
{
  SetReadoutSingleCycleSpeed(RSC_40Mbits_s); // FIXME don't care...
  SetSerialDelay(0x0); // FIXME maybe affected by the realistic tests
  SetStrobeSelect(SS_LeadingEdge);
  SetReadoutSpeedSelect(RO_Fixed);
  SetTokenDelay(0x0); // FIXME maybe affected by the realistic tests
  
  SetEnableLocalTrailer(true); // FIXME not yet discussed...
  SetEnableLocalHeader(true); // FIXME not yet discussed...
  SetEnableGlobalTrailer(true); // FIXME not yet discussed...
  SetEnableGlobalHeader(true); // FIXME not yet discussed...
  
  SetKeepToken(true);
  SetMaster(true);
  SetEnableBytewise(true);
  
  SetBypassInputs(true);
  SetReadoutFIFOSize(256);
  
  SetEnableOverflowDetect(true);
  SetEnableRelative(false);
  SetEnableAutomaticReject(false);
  
  /*SetEventCountOffset(0); // FIXME needs confirmation
  SetTriggerCountOffset(0);*/
  
  SetEnableSetCountersOnBunchReset(false); // FIXME not yet discussed...
  SetEnableMasterResetCode(false);
  SetEnableMasterResetOnEventReset(false);
  SetEnableResetChannelBufferWhenSeparator(false); // FIXME not yet discussed...
  SetEnableSeparatorOnEventReset(false);
  SetEnableSeparatorOnBunchReset(false);
  SetEnableDirectEventReset(true);
  SetEnableDirectBunchReset(true);
  SetEnableDirectTrigger(true);
  
  SetLowPowerMode(true);
  SetDLLControl(0x1);
  
  //SetDeadTime(DT_5ns); // FIXME do we force the dead time value?
  //SetTestInvert(false);
  //SetTestMode(false);
  
  SetModeRCCompression(true);
  SetModeRC(true);
  SetDLLMode(DLL_320MHz);
  SetPLLControl(0x4, false, false, false);
  
  SetSerialClockDelay(false, 0x0);
  SetIOClockDelay(false, 0x0);
  SetCoreClockDelay(false, 0x0);
  SetDLLClockDelay(false, 0x0);
  SetSerialClockSource(Serial_pll_clock_80);
  SetIOClockSource(IO_clock_40);
  SetCoreClockSource(Core_pll_clock_80);
  SetDLLClockSource(DLL_pll_clock_320);
  
  SetRollOver(0xFFF);
  SetEnableTTLSerial(true);
  SetEnableTTLControl(true);
  SetEnableTTLReset(true);
  SetEnableTTLClock(false);
  SetEnableTTLHit(false);
}

void
TDCSetup::Dump(int verb, std::ostream& os) const
{
  os << "====================="
     << " TDC Configuration dump "
     << "=====================" << std::endl;
  if (verb>1) {
    os << std::endl;
    for (unsigned int i=0; i<sizeof(fWord)/sizeof(fWord[0]); i++) {
      os << " Word " << std::setw(2) << i << ":  "
         << std::setw(3) << (i+1)*WORD_SIZE-1 << "-> |";
      for(int8_t j=WORD_SIZE-1; j>=0; j--) {
        uint16_t bit = j+i*WORD_SIZE;
        // bits values
        if (bit>=BITS_NUM) os << "x";
        else os << static_cast<bool>((fWord[i] & static_cast<word_t>(1<<j))>>j);
        // delimiters
        if (j%16==0 && j!=0) os << "| |";
        else if (j%8==0) os << "|";
        else if (j%4==0) os << " ";
      }
      os << " <-" << std::setw(3) << i*WORD_SIZE << std::endl;
    }
    os << std::endl;
  }
  os << " Enabled errors:             ";
  for (unsigned int i=0; i<11; i++) {
    if (static_cast<bool>((GetEnableError()>>i)&0x1)) os << i << " ";
  }
  os << std::endl;
  os << " Edge resolution:            " << GetEdgeResolution() << std::endl
     << " Maximal event size:         " << GetMaxEventSize() << std::endl
     << " Reject events if FIFO full? " << GetRejectFIFOFull() << std::endl
     << " Channels offset/DLL adjustments:" << std::endl
     << "   +---------------------------------------------------------+" << std::endl;
  for (unsigned int i=0; i<NUM_CHANNELS/2; i++ ) {
    os << "   |  Ch.  " << std::setw(2) << i
       << ":   0x" << std::setfill('0')
       << std::setw(3) << std::hex << static_cast<int>(GetChannelOffset(i))
       << " / 0x"
       << std::setw(3) << static_cast<int>(GetDLLAdjustment(i)) << std::dec << std::setfill(' ')
       << "   |  Ch.  " << std::setw(2) << i+NUM_CHANNELS/2
       << ":   0x" << std::setfill('0')
       << std::setw(3) << std::hex << static_cast<int>(GetChannelOffset(i+NUM_CHANNELS/2))
       << " / 0x"
       << std::setw(3) << static_cast<int>(GetDLLAdjustment(i+NUM_CHANNELS/2)) << std::dec << std::setfill(' ')
       << " |" << std::endl;
  }
  os << "   +---------------------------------------------------------+" << std::endl
     << " Width resolution:           " << GetWidthResolution() << std::endl
     << " Dead time:                  " << GetDeadTime() << std::endl
     << " Leading/trailing mode:      " << GetLeadingMode() << " / " << GetTrailingMode() << std::endl
     << " Trigger matching mode:      " << GetTriggerMatchingMode() << std::endl
     << " Edges pairing:              " << GetEdgesPairing() << std::endl;
  os << "================================="
     << "=================================" << std::endl;
}