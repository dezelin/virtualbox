/** @file
 *
 * VirtualBox guest execution control private data definitions
 */

/*
 * Copyright (C) 2011 Oracle Corporation
 *
 * This file is part of VirtualBox Open Source Edition (OSE), as
 * available from http://www.virtualbox.org. This file is free software;
 * you can redistribute it and/or modify it under the terms of the GNU
 * General Public License (GPL) as published by the Free Software
 * Foundation, in version 2 as it comes in the "COPYING" file of the
 * VirtualBox OSE distribution. VirtualBox OSE is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY of any kind.
 */

#ifndef ____H_GUESTIMPLPRIVATE
#define ____H_GUESTIMPLPRIVATE

#include "VirtualBoxBase.h"

#include <map>
#include <vector>

#ifdef VBOX_WITH_GUEST_CONTROL
# include <VBox/HostServices/GuestControlSvc.h>
using namespace guestControl;
#endif

/** Structure representing the "value" side of a "key=value" pair. */
typedef struct VBOXGUESTCTRL_STREAM_PAIR
{
    char *pszValue;
} VBOXGUESTCTRL_STREAM_PAIR, *PVBOXGUESTCTRL_STREAM_PAIR;

/** Map containing "key=value" pairs of a stream object. */
typedef std::map< Utf8Str, VBOXGUESTCTRL_STREAM_PAIR > GuestCtrlStreamPairs;
typedef std::map< Utf8Str, VBOXGUESTCTRL_STREAM_PAIR >::iterator GuestCtrlStreamPairsIter;
typedef std::map< Utf8Str, VBOXGUESTCTRL_STREAM_PAIR >::const_iterator GuestCtrlStreamPairsIterConst;

class GuestProcessStream
{

public:

    GuestProcessStream();

    virtual ~GuestProcessStream();

public:

    void Destroy();

    void ClearPairs();

    const char* GetString(const char *pszKey);

    int GetUInt32Ex(const char *pszKey, uint32_t *puVal);

    uint32_t GetUInt32(const char *pszKey);

    int GetInt64Ex(const char *pszKey, int64_t *piVal);

    int64_t GetInt64(const char *pszKey);

    size_t GetNumPairs();

    int AddData(const BYTE *pbData, size_t cbData);

    int Parse();

protected:

    /** The map containing one more more stream pairs. */
    GuestCtrlStreamPairs m_mapPairs;
    /** Currently allocated size of internal stream buffer. */
    uint32_t m_cbAllocated;
    /** Currently used size of allocated internal stream buffer. */
    uint32_t m_cbSize;
    /** Current offset within the internal stream buffer. */
    uint32_t m_cbOffset;
    /** Current parser offset. */
    uint32_t m_cbParserOffset;
    /** Internal stream buffer. */
    BYTE *m_pbBuffer;
};
#endif // ____H_GUESTIMPLPRIVATE
