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
    fd = open(fname.c_str(), O_CREAT | O_WRONLY, 0644);
}

void Writer::Wwrite(int step, const GrayScott &sim)
{
    if (!sim.size_x || !sim.size_y || !sim.size_z)
    {
        return;
    }

    std::vector<double> u = sim.u_noghost();
    std::vector<double> v = sim.v_noghost();

     //Write file into data.0
     write(fd, &step, sizeof(int));
     write(fd, u.data(), u.size() * sizeof(double));
     write(fd, v.data(), v.size() * sizeof(double));

}

void Writer::Wclose() 
{ 
    //POSIX close file
    close(fd);
    
}
