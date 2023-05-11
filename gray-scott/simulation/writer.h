#ifndef __WRITER_H__
#define __WRITER_H__

#include <adios2.h>
#include <mpi.h>

#include "../../gray-scott/simulation/gray-scott.h"
#include "../../gray-scott/simulation/settings.h"

class Writer
{
public:
    Writer(const Settings &settings, const GrayScott &sim, adios2::IO io);
    void Wopen(const std::string &fname, bool append);
    int fd = open("/home/gp.sc.cc.tohoku.ac.jp/tseng/ADIOS2/Test/Tutorial/VE/share/adios2-examples/gray-scott/gs.bp/data.0", O_CREAT | O_WRONLY, 0644);
    void Wwrite(int step, const GrayScott &sim, int fd);
    void Wclose(int fd);

protected:
    Settings settings;

    adios2::IO io;
    adios2::Engine writer;
    adios2::Variable<double> var_u;
    adios2::Variable<double> var_v;
    adios2::Variable<int> var_step;
};

#endif
