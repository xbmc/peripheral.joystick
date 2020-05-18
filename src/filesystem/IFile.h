/*
 *  Copyright (C) 2015-2020 Garrett Brown
 *  Copyright (C) 2015-2020 Team Kodi
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 *  See LICENSE.md for more information.
 */

#pragma once

#include <stdint.h>
#include <string>

namespace JOYSTICK
{
  // File flags from File.h of Kodi
  enum READ_FLAG
  {
    // Perform a normal read
    READ_FLAG_NONE    = 0x00,
    // Indicate that caller can handle truncated reads, where function returns before entire buffer has been filled
    READ_TRUNCATED    = 0x01,
    // Indicate that that caller support read in the minimum defined chunk size, this disables internal cache then
    READ_CHUNKED      = 0x02,
    // Use cache to access this file
    READ_CACHED       = 0x04,
    // Open without caching. regardless to file type.
    READ_NO_CACHE     = 0x08,
    // Calcuate bitrate for file while reading
    READ_BITRATE      = 0x10,
    // Indicate the caller will seek between multiple streams in the file frequently
    READ_MULTI_STREAM = 0x20,
  };

  // Helper function to combine read flags
  inline READ_FLAG operator|(READ_FLAG lhs, READ_FLAG rhs)
  {
    return static_cast<READ_FLAG>(static_cast<unsigned int>(lhs) | static_cast<unsigned int>(rhs));
  }

  // Seek flags from stdio.h
  enum SEEK_FLAG
  {
    SEEK_FLAG_SET = 0, // Beginning of file
    SEEK_FLAG_CUR = 1, // Current position of the file pointer
    SEEK_FLAG_END = 2, // End of file Implementation not required
  };

  class IFile
  {
  public:
    virtual ~IFile(void) { }

    /*!
     * \brief Open a file for reading
     *
     * \return true if the file was opened and is ready for reading
     */
    virtual bool Open(const std::string& url, READ_FLAG flags = READ_FLAG_NONE) = 0;

    /*!
     * \brief Open a file for writing
     *
     * \return true if the file was opened and is ready for writing
     */
    virtual bool OpenForWrite(const std::string& url, bool bOverWrite = false) = 0;

    /*!
     * \brief Read data from a file open for reading
     *
     * \return The number of bytes read, or -1 for error
     */
    virtual int64_t Read(uint64_t byteCount, std::string& buffer) = 0;

    /*!
     * \brief Read a line of text from the file
     *
     * \return The number of bytes read, or -1 for error
     */
    virtual int64_t ReadLine(std::string& buffer) = 0;

    /*!
     * \brief Read the entire file
     *
     * \param buffer The buffer to read into
     * \param maxBytes The maximum number of bytes that may be read
     *
     * \return the number of bytes read, or -1 for error
     */
    virtual int64_t ReadFile(std::string& buffer, const uint64_t maxBytes = 0) = 0;

    /*!
     * \brief Write to a file open for writing
     *
     * \return The number of bytes written, or -1 for error
     */
    virtual int64_t Write(uint64_t byteCount, const std::string& buffer) = 0;

    /*!
     * \brief Flush the file to the disk
     */
    virtual void Flush(void) { }

    /*!
     * \brief Seek in the file
     */
    virtual int64_t Seek(int64_t filePosition, SEEK_FLAG whence = SEEK_FLAG_SET) = 0;

    /*!
     * \brief Truncate the file
     *
     * \return true if the file has been truncated to the specified size
     */
    virtual bool Truncate(uint64_t size) = 0;

    /*!
     * \brief Tell the current position in the file
     *
     * \return The current position, or -1 for error
     */
    virtual int64_t GetPosition(void) = 0;

    /*!
     * \brief Get the number of bytes in the file
     *
     * \return The number of bytes, or -1 for error
     */
    virtual int64_t GetLength(void) = 0;

    /*!
     * \brief Close the file
     */
    virtual void Close(void) = 0;
  };
}
