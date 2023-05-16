/*
 * Analysis code for the Gray-Scott application.
 * Reads variable U and V, and computes the PDF for each 2D slices of U and V.
 * Writes the computed PDFs using ADIOS.
 *
 * Norbert Podhorszki, pnorbert@ornl.gov
 *
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <thread>

#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

#include "../../gray-scott/common/timer.hpp"
#include "../../gray-scott/simulation/gray-scott.h"
#include "../../gray-scott/simulation/writer.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <mpi.h>

/*
 * MAIN
 */
int main(int argc, char *argv[])
{
    int provided;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    int rank, comm_size, wrank;

    MPI_Comm_rank(MPI_COMM_WORLD, &wrank);

    const unsigned int color = 2;
    MPI_Comm comm;
    MPI_Comm_split(MPI_COMM_WORLD, color, wrank, &comm);

    MPI_Comm_rank(comm, &rank);
    MPI_Comm_size(comm, &comm_size);

    if (argc < 3)
    {
        std::cout << "Not enough arguments\n";
        MPI_Finalize();
        return 0;
    }

    if (argc >= 4)
    {
        int value = std::stoi(argv[3]);
    }

    if (argc >= 5)
    {
    }

    //Settings settings = Settings::from_json(argv[1]);

    //GrayScott sim(settings, comm);
    //sim.init();
    
#ifdef ENABLE_TIMERS
    Timer timer_total;
    Timer timer_read;
    Timer timer_compute;
    Timer timer_write;

    std::ostringstream log_fname;
    log_fname << "analysis_pe_" << rank << ".log";

    std::ofstream log(log_fname.str());
    log << "step\ttotal_gs\tread_gs\t\tcompute_gs\twrite_gs" << std::endl;
#endif

    {
        int step;
        int fr= open("gs.bp", O_RDONLY, 0644);
        int fw= open("pdf.bp", O_CREAT | O_WRONLY, 0644);
        if (fr==-1)
        {
            printf("Can't read gs.bp\n");
            exit(1);
        }
        if (fw==-1)
        {
            printf("Can't write pdf.bp\n");
            exit(1);
        }


        // read data per timestep
        int stepAnalysis = 0;
        bool go=true;
        while (go==true)
        {

#ifdef ENABLE_TIMERS
        MPI_Barrier(comm);
        timer_total.start();
        timer_read.start();
#endif


            std::vector<double> u;
            std::vector<double> v;
            //// Read adios2 data
            read(fr,&step, sizeof(int));
            read(fr,u.data(), u.size() * sizeof(double));
            read(fr,v.data(), v.size() * sizeof(double));

#ifdef ENABLE_TIMERS
        double time_read = timer_read.stop();
        MPI_Barrier(comm);
        timer_compute.start();
#endif

            if (!rank)
            {
                std::cout << "PDF Analysis step " << stepAnalysis << std::endl; //<< std::endl
                std::cout << u.at(1) << ' ' << "This is v";
                
                for(int i=0; i < v.size(); i++)
                {
                    std::cout << v.at(i) << ' ' << "This is v"<< std::endl;
                }
                std::cout << "Test " << stepAnalysis << std::endl; //<< std::endl
            }

#ifdef ENABLE_TIMERS
        timer_compute.stop();
        MPI_Barrier(comm);
        timer_write.start();
#endif

            //writer.EndStep();
            ++stepAnalysis;
        
        //Write file into data.0
        write(fw, &step, sizeof(int));
        write(fw, u.data(), u.size() * sizeof(double));
        write(fw, v.data(), v.size() * sizeof(double));
            
#ifdef ENABLE_TIMERS
        double time_write = timer_write.stop();
        double time_step = timer_total.stop();
        MPI_Barrier(comm);

        log << stepAnalysis << "\t" << "\t" << timer_total.elapsed() << "\t" << "\t" << timer_read.elapsed() << "\t" << "\t"
            << timer_compute.elapsed() << "\t" << "\t" << timer_write.elapsed()
            << std::endl;
#endif
        if (++stepAnalysis==1000)
        {
            go=false;
        }
        }

        //// cleanup
        //reader.Close();
        //writer.Close();
        close(fr);
        close(fw);
    }

    MPI_Barrier(comm);
    
#ifdef ENABLE_TIMERS
    log << "total\t" << timer_total.elapsed() << "\t" << timer_read.elapsed() << "\t" << timer_compute.elapsed()
        << "\t" << timer_write.elapsed() << std::endl;

    log.close();
#endif

    MPI_Finalize();
    //const sec duration = clock::now() - before;
    //std::cout << "It took " << duration.count() << "s" << std::endl;
    return 0;
}
