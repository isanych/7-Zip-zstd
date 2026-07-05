// TarPrefixCheck.h

#ifndef ZIP7_INC_ARCHIVE_TAR_PREFIX_CHECK_H
#define ZIP7_INC_ARCHIVE_TAR_PREFIX_CHECK_H

#include "../../../Common/MyCom.h"
#include "../../ICoder.h"

#include "../../Common/StreamObjects.h"

#include "../Tar/TarIn.h"

// Shared by the single-stream compressor handlers (gzip/bzip2/xz/zstd) that
// support one-step tar-dialect extraction (kpidMainSubfile + GetStream, see
// CArchiveLink::Open in OpenArchive.cpp). Passed as the progress callback
// to a decoder's CodeResume() loop: aborts decoding via E_ABORT as soon as
// enough decompressed bytes are available to tell whether the content is a
// tar, so opening a plain non-tar archive of this type never fully decodes
// it just because it was opened (l / t / x). Once tar is confirmed, it
// still caps total decoded size so an unbounded tar-dialect archive can't
// be fully pulled into RAM just because it was opened.
//
// Header-only (not .h/.cpp) so it can be included from multiple handler
// .cpp files without needing every consumer's Makefile object list to
// gain a new dependency -- SetRatioInfo below is `inline` accordingly.
Z7_CLASS_IMP_COM_1(
  CTarPrefixCheckProgress
  , ICompressProgressInfo
)
public:
  CDynBufSeqOutStream *Buf;
  UInt32 TarCheck;
  UInt64 MaxSize;
  CTarPrefixCheckProgress(): Buf(NULL), TarCheck(k_IsArc_Res_NEED_MORE), MaxSize((UInt64)1 << 32) {}

  // Some decoders (e.g. Deflate's CCoder::CodeReal()) skip the final
  // SetRatioInfo() call for the exact chunk that finishes decoding, so for
  // small inputs (whole content decoded in one chunk) SetRatioInfo may
  // never run and TarCheck stays NEED_MORE even though decoding is done.
  // Call this once after CodeResume()/Code() returns to cover that case.
  void FinalCheck()
  {
    if (TarCheck == k_IsArc_Res_NEED_MORE)
      TarCheck = NArchive::NTar::IsArc_Tar(Buf->GetBuffer(), Buf->GetSize());
  }
};

inline Z7_COM7F_IMF(CTarPrefixCheckProgress::SetRatioInfo(const UInt64 * /*inSize*/, const UInt64 * /*outSize*/))
{
  if (TarCheck == k_IsArc_Res_NEED_MORE)
  {
    TarCheck = NArchive::NTar::IsArc_Tar(Buf->GetBuffer(), Buf->GetSize());
    if (TarCheck == k_IsArc_Res_NO)
      return E_ABORT;
  }
  if (Buf->GetSize() > MaxSize)
    return E_ABORT;
  return S_OK;
}

#endif
