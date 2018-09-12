// Copyright (C) 2018 David Reid. See included LICENSE file.

template <typename T> T ocTypeMin() { return 0; }
template <> ocInt8    ocTypeMin() { return INT8_MIN;   }
template <> ocUInt8   ocTypeMin() { return 0;          }
template <> ocInt16   ocTypeMin() { return INT16_MIN;  }
template <> ocUInt16  ocTypeMin() { return 0;          }
template <> ocInt32   ocTypeMin() { return INT32_MIN;  }
template <> ocUInt32  ocTypeMin() { return 0;          }
template <> ocInt64   ocTypeMin() { return INT64_MIN;  }
template <> ocUInt64  ocTypeMin() { return 0;          }
template <> ocFloat32 ocTypeMin() { return -FLT_MAX;   }
template <> ocFloat64 ocTypeMin() { return -DBL_MAX;   }

template <typename T> T ocTypeMax() { return 0; }
template <> ocInt8    ocTypeMax() { return INT8_MAX;   }
template <> ocUInt8   ocTypeMax() { return UINT8_MAX;  }
template <> ocInt16   ocTypeMax() { return INT16_MAX;  }
template <> ocUInt16  ocTypeMax() { return UINT16_MAX; }
template <> ocInt32   ocTypeMax() { return INT32_MAX;  }
template <> ocUInt32  ocTypeMax() { return UINT32_MAX; }
template <> ocInt64   ocTypeMax() { return INT64_MAX;  }
template <> ocUInt64  ocTypeMax() { return UINT64_MAX; }
template <> ocFloat32 ocTypeMax() { return FLT_MAX;    }
template <> ocFloat64 ocTypeMax() { return DBL_MAX;    }