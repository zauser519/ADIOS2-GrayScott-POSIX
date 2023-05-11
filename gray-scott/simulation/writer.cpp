#include "../../gray-scott/simulation/writer.h"
 
#include <string>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

Writer::Writer(const Settings &settings, const GrayScott &sim)
: settings(settings)
{
    
}

void Writer::Wopen(const std::string &fname)
{
    int fd = open(fname, O_CREAT | O_WRONLY, 0644);
}

void Writer::Wwrite(int step, const GrayScott &sim, int fd)
{
    if (!sim.size_x || !sim.size_y || !sim.size_z)
    {
        return;
    }

    std::vector<double> u = sim.u_noghost();
    std::vector<double> v = sim.v_noghost();

     //Write file into data.0
     write(fd, &step, sizeof(int));
     write(fd, u.data(), sizeof(u.size() * sizeof(double)));
     write(fd, v.data(), sizeof(v.size() * sizeof(double)));

}

void Writer::Wclose(int fd) 
{ 
    //POSIX close file
    close(fd);
    
}
