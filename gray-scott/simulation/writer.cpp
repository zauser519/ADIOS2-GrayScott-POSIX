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


void Writer::Wwrite(int step, const GrayScott &sim, MPI_Comm comm, int rank)
{
    if (!sim.size_x || !sim.size_y || !sim.size_z)
    {
        return;
    }

    std::vector<double> u = sim.u_noghost();
    std::vector<double> v = sim.v_noghost();

    //pointer
    perrank=0;
    writen_thisstep=0;
    
    writen_thisprocessor = u.size() * sizeof(double) + v.size() * sizeof(double);
    MPI_Exscan(&writen_thisprocessor, &perrank, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
    //prevent write repeatly
    if (perrank==0){
      write(fd, &step, sizeof(int));
    }
    lseek(fd, sizeof(int)*step+perrank+perstep, SEEK_SET);
    write(fd, u.data(), u.size() * sizeof(double));
    write(fd, v.data(), v.size() * sizeof(double));
    MPI_Allreduce(&writen_thisprocessor, &writen_thisstep, 1, MPI_UNSIGNED_LONG, MPI_SUM, MPI_COMM_WORLD);
    if(perrank==0){
        std::cout << "The sum of the size of previous time steps is " << perstep << std::endl;
    std::cout << "In rank " << rank  
      << " u size is " << u.size() * sizeof(double)
      << " v size is " << v.size() * sizeof(double)
      << " step " << sizeof(int)
      << " Total size is " << writen_thisprocessor << std::endl;
    std::cout << "Currently, in this time step, before processor " << rank
      << " it wrote " << perrank << std::endl;
    std::cout << "The result of processor "<< rank
      << " writes to " << perrank + perstep << std::endl;
        std::cout << "This time step has size of " << writen_thisstep << std::endl;
    }else
    {
      std::cout << "In rank " << rank  
        << " u size is " << u.size() * sizeof(double)
        << " v size is " << v.size() * sizeof(double)
        << " Total size is " << writen_thisprocessor << std::endl;
    std::cout << "Currently, in this time step, before processor " << rank
      << " it wrote " << perrank << std::endl;
    std::cout << "The result of processor "<< rank
      << " writes to " << perrank + perstep << std::endl;
    }
    perstep += writen_thisstep;


}

void Writer::Wclose() 
{ 
    //POSIX close file
    close(fd);
    
}
