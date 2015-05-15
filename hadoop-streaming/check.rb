#!/usr/bin/ruby

require 'socket'

processing_dir = "/mnt/hdfs/data/nxcore/processed"
processing = Dir["#{processing_dir}/*.tmp"]
processed = Dir["#{processing_dir}/*.bz2"]
filelists = Dir["/mnt/hdfs/data/nxcore/filelist/*.txt"]

# puts "#{processed.to_s}\n"

count = 0

filelists.each do | filelist |

   count = count + 1

   processed_file = ""
   processing_file = ""

   host = filelist.scan(/(r\d\d\du\d\d)/)

   # puts "host: #{host}\n"

   File.open(filelist,'r').readlines.each do | file |

        # puts "\tfile: #{file}\n"

	processing_filename = "#{processing_dir}/#{file.strip}.txt.bz2.tmp"
        processed_filename = "#{processing_dir}/#{file.strip}.bz2"
       
        # puts "#{processed_filename}\n"

        if processed.include? processed_filename 
           processed_file = processed_filename
        elsif processing.include? processing_filename
           processing_file = processing_filename
        else
           # due nothing          
        end
   end

   if processing_file != ""
      puts "#{host}: still processing :#{processing_file}. Last processed file: #{processed_file}\n"
   elsif processed_file != ""
      puts "#{host}: last processed file - #{processed_file}. Process is done.\n"
   else
      puts "#{host}: cannot find any processed files\n"
   end

   # break if count == 3 

end
