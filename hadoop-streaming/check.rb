#!/usr/bin/ruby

require 'socket'

processing_dir = "/mnt/hdfs/data/nxcore/processed/2014/08"
processing = Dir["#{processing_dir}/*.tmp"]
processed = Dir["#{processing_dir}/*.bz2"]
to_process = Dir["/mnt/hdfs/SummerCamp2015/nxcore/raw/dataset1/*.*"]
filelists = Dir["/mnt/hdfs/data/nxcore/filelist/2014/08/*.txt"]

puts "total file to process: #{to_process.size}\n"
puts "files processed: #{processed.size}\n"
puts "files processing: #{processing.size}\n"
puts "files remaining: #{to_process.size-processed.size-processing.size}\n"
puts "configuration files: #{filelists.size}\n"

# puts "#{processed.to_s}\n"

count = 0

filelists.each do | filelist |

   count = count + 1

   processed_file = ""
   processing_file = ""
   processing_file_index = -1

   host = filelist.scan(/(r\d\d\du\d\d)/)

   # puts "host: #{host}\n"

   file_count = 0

   File.open(filelist,'r').readlines.each_with_index do | file, index |

        file_count = file_count + 1

        # puts "\tfile: #{file}\n"

	processing_filename = "#{processing_dir}/#{file.strip}.txt.bz2.tmp"
        processed_filename = "#{processing_dir}/#{file.strip}.bz2"
       
        # puts "#{processed_filename}\n"

        if processed.include? processed_filename 
           processed_file = processed_filename
           processing_file_index = index + 1
        elsif processing.include? processing_filename
           processing_file = processing_filename
           processing_file_index = index + 1
        else
           # due nothing          
        end
   end

   if processing_file != ""
      puts "#{host}: still processing #{processing_file_index} of #{file_count} :#{processing_file}. Last processed file: #{processed_file}\n"
   elsif processed_file != ""
      puts "#{host}: last processed file - #{processed_file}. Process is done. #{file_count} files total.\n"
   else
      puts "#{host}: cannot find any processed files\n"
   end

   # break if count == 3 

end
