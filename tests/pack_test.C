#include <sys/time.h>
#include <cstdlib>
#include <vector>
#include <set>
#include <mpi.h>
#include "Callpath.h"

using namespace std;

const size_t num_callpaths  = 10000;
const size_t average_length = 100;
const size_t variation = 10;


double get_time_sec() {
  static struct timeval tv;
  gettimeofday(&tv, 0);
  return tv.tv_sec + tv.tv_usec / 1e6;
}

double last_time;
void start() { 
  last_time = get_time_sec();
}

double delta() {
  double time = get_time_sec();
  double delta = time - last_time;
  last_time = time;
  return delta;
}

const char *modules[] = {
  "/usr/lib/libsvn_repos-1.0.dylib",
  "/usr/lib/libsvn_fs-1.0.dylib",
  "/usr/lib/libsvn_fs_fs-1.0.dylib",
  "/usr/lib/libsvn_delta-1.0.dylib",
  "/usr/lib/libsvn_fs_util-1.0.dylib",
  "/usr/lib/libsvn_subr-1.0.dylib",
  "/usr/lib/libiconv.2.dylib",
  "/usr/lib/libsqlite3.dylib",
  "/usr/lib/libapr-1.0.dylib",
  "/usr/lib/libSystem.B.dylib",
  "/usr/lib/libaprutil-1.0.dylib",
  "/usr/lib/libz.1.dylib",
  "/usr/lib/libexpat.1.dylib",
  "/System/Library/Frameworks/Security.framework/Versions/A/Security",
  "/System/Library/Frameworks/CoreServices.framework/Versions/A/CoreServices"
};
const size_t num_modules = sizeof(modules) / sizeof(char*);



int main(int argc, char **argv) {
  MPI_Init(&argc, &argv);
  srandom(100);

  // construct a synthetic set of callpaths.
  vector<Callpath> paths;
  for (size_t i=0; i < num_callpaths; i++) {
    int len = average_length;
    len += (int)(random() / (double)RAND_MAX * variation - (variation/2.0));

    vector<FrameId> frames;
    for (int f=0; f < len; f++) {
      size_t m = (size_t)(random() / (double)RAND_MAX * num_modules);
      frames.push_back(FrameId(modules[m], random()));
    }

    Callpath cp = Callpath::create(frames);
    paths.push_back(cp);
  }
  
  
  start();
  MPI_Comm comm = MPI_COMM_WORLD;
  size_t modsize = ModuleId::packed_size_id_map(comm);
  cout << "Modules packed size    " << delta() << endl;
  
  size_t path_size = 0;
  for (size_t i=0; i < paths.size(); i++) {
    path_size += paths[i].packed_size(comm);
  }
  cout << "Callpaths packed size  " << delta() << endl;
  
  int buf_size = modsize + path_size;
  vector<char*> buffer(buf_size);
  int pos = 0;
  ModuleId::pack_id_map(&buffer[0], buf_size, &pos, comm);
  cout << "Modules pack           " << delta() << endl;
  
  for (size_t i=0; i < paths.size(); i++) {
    paths[i].pack(&buffer[0], buf_size, &pos, comm);
  }
  cout << "Callpaths pack         " << delta() << endl;  
  
  ModuleId::id_map modules;
  int unpack_pos = 0;
  ModuleId::unpack_id_map(&buffer[0], buf_size, &unpack_pos, modules, comm);
  cout << "Modules unpack         " << delta() << endl;    
  
  vector<Callpath> unpacked_paths;
  for (size_t i=0; i < paths.size(); i++) {
    Callpath path = Callpath::unpack(modules, &buffer[0], buf_size, &unpack_pos, comm);
    unpacked_paths.push_back(path);
  }
  cout << "Callpaths unpack       " << delta() << endl;    

  
  // make sure unpacked paths are the same as packed ones.
  cout << endl;
  cout << "Unpacked " << unpacked_paths.size() << " callpaths." << endl;
  
  bool same = true;
  for (size_t i = 0; i < paths.size(); i++) {
    if (paths[i] != unpacked_paths[i]) {
      cout << "warning: path[i] != unpacked_paths[i]" << endl;
      same = false;
    }
  }
  if (same) {
    cout << "Validated callpaths." << endl;
  }

  MPI_Finalize();
}

