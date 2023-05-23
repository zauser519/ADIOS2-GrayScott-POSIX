#ifndef __WRITER_H__
#define __WRITER_H__

#include <adios2.h>
#include <mpi.h>

#include "../../gray-scott/simulation/gray-scott.h"
#include "../../gray-scott/simulation/settings.h"

class Writer
{
public:
    Writer(const Settings &settings, const GrayScott &sim);
    void Wopen(const std::string &fname);
    void Wwrite(int step, const GrayScott &sim, MPI_Comm comm, int rank);
    void Wclose();

protected:
    //write mode
    int fd;
    //exscan, for each step
    int perrank, writen_thisprocessor;
    //allreduce, for steps
    long long perstep=0, writen_thisstep;
    Settings settings;
};

#endif
