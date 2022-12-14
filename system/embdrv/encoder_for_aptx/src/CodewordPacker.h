/*------------------------------------------------------------------------------
 *
 *  Prototype declaration of the CodewordPacker Function
 *
 *  This functions allows a client to supply an array of 4 quantised codes
 *  (1 per subband) and obtain a packed version as a 16-bit aptX codeword.
 *
 *----------------------------------------------------------------------------*/

#ifndef CODEWORDPACKER_H
#define CODEWORDPACKER_H

#include "AptxParameters.h"

XBT_INLINE_ int16_t packCodeword(Encoder_data* EncoderDataPt,
                                 uint32_t aligned) {
  int32_t syncContribution;
  int32_t hhCode;
  int32_t codeword;

  /* The per-channel contribution to derive the current sync bit is the XOR of
   * the 4 code lsbs and the random dither bit. The SyncInserter engineers it
   * such that the XOR of the sync contributions from the left and right
   * channel give the actual sync bit value. The per-channel sync bit
   * contribution overwrites the HH code lsb in the packed codeword. */
  if (aligned != no_sync) {
    syncContribution =
        (EncoderDataPt->m_qdata[0].qCode ^ EncoderDataPt->m_qdata[1].qCode ^
         EncoderDataPt->m_qdata[2].qCode ^ EncoderDataPt->m_qdata[3].qCode ^
         EncoderDataPt->m_dithSyncRandBit) &
        0x1;
    hhCode = (EncoderDataPt->m_qdata[HH].qCode & 0x6) | syncContribution;

    /* Pack the 16-bit codeword with the appropriate number of lsbs from each
     * quantised code (LL=7, LH=4, HL=2, HH=3). */
    codeword = (EncoderDataPt->m_qdata[LL].qCode & 0x7fL) |
               ((EncoderDataPt->m_qdata[LH].qCode & 0xfL) << 7) |
               ((EncoderDataPt->m_qdata[HL].qCode & 0x3L) << 11) |
               (hhCode << 13);
  } else {  // don't add sync contribution for non-autosync mode
    codeword = (EncoderDataPt->m_qdata[LL].qCode & 0x7fL) |
               ((EncoderDataPt->m_qdata[LH].qCode & 0xfL) << 7) |
               ((EncoderDataPt->m_qdata[HL].qCode & 0x3L) << 11) |
               ((EncoderDataPt->m_qdata[HH].qCode & 0x7L) << 13);
  }
  return (int16_t)codeword;
}

#endif  // CODEWORDPACKER_H
