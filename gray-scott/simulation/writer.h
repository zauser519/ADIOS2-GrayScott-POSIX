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
    void Wwrite(int step, const GrayScott &sim);
    void Wclose();

protected:
    int fd;
    Settings settings;
};

#endif
