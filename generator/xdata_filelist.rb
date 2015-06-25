#!/usr/bin/ruby
#
#       AUTHOR: Matt Parker
# LAST UPDATED: 6/24/2015
#  DESCRIPTION: Program assigns raw NXCore data files to machines for 
#               parallel data extraction. 
#
# Usage: filelisting.rb [node file] [hdfs file listing] [output directory]
#
#          node file - Text file containing the Linux hostsnames used for processing.
#  hdfs file listing - Text file containing the contents of the following commannd:
#                          usage: hdfs dfs -ls [HDFS directory]
#   output directory - Directory containing a set of text files containing 
#
######################################################################################

node_file_list = ARGV[0]
datanodes = File.readlines( node_file_list )

files = Hash.new

hdfs_file_listing = ARGV[1]
output_directory = ARGV[2]

   #Echo parameters
puts "\nNode file list: #{node_file_list}\n"
puts "HDFS file listing file: #{hdfs_file_listing}\n"
puts "Output directory: #{output_directory}\n"

   # Associate file names and size. Sort by size.
filelist = File.open( hdfs_file_listing, "r").read

filelist.each_line do |line|
   #puts "hdfs listing line: #{line}"
   metadata = line.scan(/(\d{5,10}).*?\/([^\/]\d+.ZD.nxc)$/).to_a
   #puts "data - size: #{metadata[0][0]} filename: #{metadata[0][1].strip}\n"
   files[metadata[0][0].to_i] = metadata[0][1]
end

files_by_size = files.sort_by { |size, filepath| size }

count = 0
datanode_count = datanodes.size()
direction = 1

puts "Data node count: #{datanode_count}\n\n"

   # Assign files to data nodes for processing. A naive heuristic
   # assigns files to a data node. We a pass through the machine names
   # and assign them one by one then in reverse on the way back.
files_by_size.each do | size, filepath |

   #puts "file metadata: #{size}, #{filepath}\n"
   
   index = count % datanode_count

   if direction > 0 
      #puts "direction #{direction} index: #{index} output filename: #{datanodes[index]}\n"
      output_filename = "#{output_directory}/#{datanodes[index].chomp}.txt"
   else
      #puts "datanode count - datanode count: #{datanode_count} index: #{index} from_back: #{datanode_count-index}\n"
      output_filename = "#{output_directory}/#{datanodes[datanode_count-index-1].chomp}.txt"
   end

   puts "Assigning #{filepath} => #{output_filename}\n"

   file = File.open(output_filename,"a")
   file.puts filepath
   count = count + 1

   if count % datanode_count == 0
      direction = direction * -1
   end
end

puts "\n#{count} files were prepared for processing.\n"

