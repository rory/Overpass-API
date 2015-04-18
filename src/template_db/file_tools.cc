/** Copyright 2008, 2009, 2010, 2011, 2012 Roland Olbricht
*
* This file is part of Template_DB.
*
* Template_DB is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as
* published by the Free Software Foundation, either version 3 of the
* License, or (at your option) any later version.
*
* Template_DB is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with Template_DB.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "dispatcher.h"
#include "file_tools.h"

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>


void copy_file(const std::string& source, const std::string& dest)
{
  if (!file_exists(source))
    return;
  
  Raw_File source_file(source, O_RDONLY, S_666, "Dispatcher:1");
  uint64 size = source_file.size("Dispatcher:2");
  Raw_File dest_file(dest, O_RDWR|O_CREAT, S_666, "Dispatcher:3");
  dest_file.resize(size, "Dispatcher:4");
  
  Void_Pointer< uint8 > buf(64*1024);
  while (size > 0)
  {
    size = read(source_file.fd(), buf.ptr, 64*1024);
    dest_file.write(buf.ptr, size, "Dispatcher:5");
  }
}


std::string getcwd()
{
  int size = 256;
  char* buf;
  while (true)
  {
    buf = (char*)malloc(size);
    errno = 0;
    buf = getcwd(buf, size);
    if (errno != ERANGE)
      break;
    
    free(buf);
    size *= 2;
  }
  if (errno != 0)
  {
    free(buf);
    throw File_Error(errno, "wd", "Dispatcher::getcwd");
  }
  std::string result(buf);
  free(buf);
  if ((result != "") && (result[result.size()-1] != '/'))
    result += '/';
  return result;
}


Blocking_Client_Socket::Blocking_Client_Socket
  (int socket_descriptor_) : socket_descriptor(socket_descriptor_), state(waiting) {}


uint32 Blocking_Client_Socket::get_command()
{
  if (state == disconnected)
    return Dispatcher::HANGUP;
  else if (state == processing_command)
    return last_command;
  
  int bytes_read = recv(socket_descriptor, &last_command, sizeof(uint32), 0);
  if (bytes_read == -1)
    return 0;
  else if (bytes_read == 0)
  {
    state = disconnected;
    return Dispatcher::HANGUP;
  }
  else
  {
    state = processing_command;
    return last_command;
  }
}


std::vector< uint32 > Blocking_Client_Socket::get_arguments(int num_arguments)
{
  std::vector< uint32 > result;
  if (state == disconnected || state == waiting)
    return result;
  
  for (int i = 0; i < num_arguments; ++i)
  {
    // Wait for each argument up to 0.1 seconds
    result.push_back(0);
    int bytes_read = recv(socket_descriptor, &result.back(), sizeof(uint32), 0);
    uint counter = 0;
    while (bytes_read == -1 && counter <= 100)
    {
      bytes_read = recv(socket_descriptor, &result.back(), sizeof(uint32), 0);
      millisleep(1);
      ++counter;
    }
    if (bytes_read == 0)
    {
      state = disconnected;
      result.clear();
      break;
    }
    if (bytes_read == -1)
      break;
  }
  return result;
}


void Blocking_Client_Socket::clear_state()
{
  if (state == disconnected || state == waiting)
    return;
  
  // remove any pending data. The connection should be clear at the end of the command.
  uint32 dummy;
  int bytes_read = recv(socket_descriptor, &dummy, sizeof(uint32), 0);
  while (bytes_read > 0)
    bytes_read = recv(socket_descriptor, &dummy, sizeof(uint32), 0);
  
  if (bytes_read == 0)
  {
    state = disconnected;
    return;
  }
  state = waiting;
}


void Blocking_Client_Socket::send_result(uint32 result)
{
  if (state == disconnected || state == waiting)
    return;
  
  clear_state();

  if (state == disconnected)
    return;
  
  send(socket_descriptor, &result, sizeof(uint32), 0);
  state = waiting;
}


Blocking_Client_Socket::~Blocking_Client_Socket()
{
  close(socket_descriptor);
}
