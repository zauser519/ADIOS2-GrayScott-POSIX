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
    //perstep = 0;
}

void Writer::Wwrite(int step, const GrayScott &sim, MPI_Comm comm, int rank)
{
    if (!sim.size_x || !sim.size_y || !sim.size_z)
    {
        return;
    }

    std::vector<double> u = sim.u_noghost();
    std::vector<double> v = sim.v_noghost();
    

    //Write file into data.0
    //pointer
 
    //Initializer, perrank is the sum of the result of processors written so far in single time step
    perrank=0;
    //Pointer before write, perrank gives how far the current time step written, perstep gives sum of written result before this step steps so far
    lseek(fd, perrank + perstep*step/settings.plotgap, SEEK_SET);
    //Write
    write(fd, &step, sizeof(int));
    write(fd, u.data(), u.size() * sizeof(double));
    write(fd, v.data(), v.size() * sizeof(double));
    //the size of one processor written
    writen_thisprocessor = u.size() * sizeof(double) + v.size() * sizeof(double) + sizeof(int);
    //get the sum of the result of processors written so far
    MPI_Exscan(&writen_thisprocessor, &perrank, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    //writen_thisstep is the sum of wriiten result this time step
    MPI_Allreduce(&perrank, &writen_thisstep, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
    std::cout << "perrank " << perrank
      << " perstep " << perstep 
      << " in rank " << rank  
      << " u size " << u.size() * sizeof(double)
      << " v size " << v.size() * sizeof(double)
      << " step size " << sizeof(int)
      << " Allreduce " << writen_thisstep
      << " Total size" << writen_thisprocessor << std::endl;
    //I still can't find how to make Allreduce produce the final position of a single step, so... it's hard coding
    perstep += writen_thisstep*4/6;


}

void Writer::Wclose() 
{ 
    //POSIX close file
    close(fd);
    
}
