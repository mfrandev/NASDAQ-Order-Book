## Usage
<ol>
    <li>
    This program can run in four modes:
    <ol>
        <li><b>seq</b>: Single-threaded sequential execution</li>
        <li><b>locking_spsc</b>: Single-producer/single-consumer multithreaded model using a mutex/condition variable based queue</li>
        <li><b>boost_lf_spsc</b>: Single-producer/single-consumer multithreaded model using the Boost library's lockfree queue structure</li>
        <li><b>er_lf_spsc</b>: Single-producer/single-consumer multithreaded model using Erik Rigtorp's lockfree queue structure</li>
    </ol>
    </li>
    <li>To run the program, execute the script: <b>./buildandrunit.sh</b></li>
    <li>By default (no -m option), the program runs in <b>seq</b> mode. To change the operating mode, supply one of the above modes using the "-m [mode]" option to the "./nasdaq_order_book" executible in the "runit.sh" script.</li>
    <li>By default, the program will parse the file <b>ITCHFiles/01302019.NASDAQ_ITCH50</b>. To supply an additional file for parsing, pass the "-f [filepath]" option to the "./nasdaq_order_book" executible in the "runit.sh" script. Filepaths should follow the format: <b>../../ITCHFiles/[ITCH_FILE]</b>.</li>
</ol>

## 3rd Party Libraries
This program has several third party library requirements.
<ol>
    <li><a href="https://www.boost.org/doc/libs/latest/doc/html/lockfree.html">Boost</a>: For the lockfree, concurrent Single-Producer/Single-Consumer queue (boost::lockfree::spsc_queue). <b>Note that installation is assumed at the device level, so having a valid Boost installation is a prerequisite of running this program locally.</b></li>
    <li><a href="https://github.com/CLIUtils/CLI11">CLI11</a>: Command line argument parsing library. I use the All-in-one local header, located at lib/CLI11/CLI11.hpp.</li>
    <li><a href="https://github.com/gabime/spdlog">spdlog</a>: A fast C++ logging library used for logging to the console and writing to output files. Located at lib/spdlog.</li>
    <li>
        I also tested three specialty hashmap libraries for possible performance improvements upon std::unordered_map.
        <ol>
            <li><a href="https://abseil.io/docs/cpp/guides/container">abseil</a>: The <b>flat_hash_map</b> and <b>node_hash_map</b> structures are designed to outperform std::unordered_map in general use cases.</li>
            <li><a href="https://github.com/martinus/unordered_dense">unordered_dense::map</a>: This map is similar to abseil::flat_hash_map, but has relatively slow deletion times.</li>
            <li><a href="https://github.com/Tessil/robin-map/tree/master">robin-map</a>: This map is also similar to the above two maps, but uses robin hood hashing and supports memory alignment flexibility to store the key/value pair and hash together.</li>
        </ol>
    </li>
</ol> 

## Implementation Notes
Blog post coming soon: https://medium.com/@mfdev99.

## Hardware I've Tested On
<ol>
    <li>
        Main Workstation:
        <ul>
            <li>
                CPU Details:
                <ul>
                    <li>
                        Architecture: x86-64
                        <ul>
                            <li>CPU op-mode(s): 32-bit, 64-bit</li>
                            <li>Address sizes: 46 bits physical, 48 bits virtual.</li>
                            <li>Byte Order: Little Endian</li>
                        </ul>
                    </li>
                    <li>
                        CPU(s): 20 
                        <ul>
                            <li>On-line CPU(s) list: 0-19</li>
                        </ul>
                    </li>
                    <li>
                        Vendor ID: GenuineIntel
                        <ul>    
                            <li>Model Name: 12th Gen Intel(R) Core(TM) i7-12700</li>
                            <li>Threads per core: 2</li>
                            <li>Cores per socket: 12</li>
                            <li>Sockets: 1</li>
                            <li>CPU scaling MHz: 24%</li>
                            <li>CPU max MHz: 4900.0000</li>
                            <li>CPU min MHz: 800.0000</li>
                        </ul>
                    </li>
                    <li>
                        Caches (sum of all):
                        <ul>
                            <li>L1d: 512 KiB (12 instances)</li>
                            <li>L1i: 512 KiB (12 instances)</li>
                            <li>L2: 12 MiB (9 instances)</li>
                            <li>L3: 25 MiB (1 instance)</li>
                        </ul>
                    </li>
                </ul>
            </li>
            <li>
                Operating System: Fedora release 42 (Adams)
                <ul>
                    <li>Kernel Release: 6.15.6-200.fc42.x86_64</li>
                    <li>Kernal Version: #1 SMP PREEMPT_DYNAMIC Thu Jul 10 15:22:32 UTC 2025</li>
                    <li>GNU libc Version: 2.41</li>
                    <li>C++ Compiler: GCC 15.1.1</li>
                    <li>Filesystem Type: btrfs</li>
                </ul>
            </li>
            <li>
                RAM: 31 GiB DDR5 
                <ul>
                    <li>Data Width: 64 bits</li>
                    <li>Speed: 4800 MT/s</li>
                    <li>Maufacturer: Hynix</li>
                </ul>
            </li>
        </ul>
    </li>
</ol>
