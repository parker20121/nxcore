package mil.darpa.nxcore;

import java.io.IOException;
import org.apache.commons.compress.archivers.dump.DumpArchiveConstants;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.compress.BZip2Codec;
import org.apache.hadoop.io.compress.CompressionCodec;
import org.apache.hadoop.io.compress.SnappyCodec;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.mapreduce.lib.input.TextInputFormat;
import org.apache.hadoop.mapreduce.lib.output.LazyOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.MultipleOutputs;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.log4j.Logger;


/**
 * MapReduce class used to split the NXCore Windows output running on Linux.
 * 
 * @author Matt Parker (matthew.parker@l-3com.com)
 * 
 */
public class SplitFiles {
    
    private static final Logger log = Logger.getLogger(SplitFiles.class);
    
    public static final String NAME = "split-nxcore-files";
        
    /**
     * Spilt the file data into the four separate data types.
     */
    public static class SplitFilesMapper extends Mapper<LongWritable, Text, Text, Text>{

        private static final Logger log = Logger.getLogger(SplitFilesMapper.class);
        
        public static final int RECORD_DATE = 0;
        public static final int RECORD_TYPE = 0;
        public static final int RECORD = 1;
        
        @Override
        protected void map( LongWritable key, Text value, Context context ) throws IOException, InterruptedException {  
            
                //Skip header.
            if ( key.get() > 1 ){
                
                    //Split record type, record
                String[] components = value.toString().split("\\t",2);     

                if ( components.length == 2 ){
                        //Go from 20140101.XA.nvc.txt.bz2 -> 20140101
                    String filename = ((FileSplit) context.getInputSplit()).getPath().getName();
                    String[] filenameComponents = filename.split("\\.");
                    String filekey = filenameComponents[RECORD_DATE] + "-" + components[RECORD_TYPE];

                        //Segment data by file.           
                    context.write( new Text(filekey), new Text(components[RECORD]) ); 
                } else {
                    System.err.println("Error processing line " + key.toString() + " in " + ((FileSplit) context.getInputSplit()).getPath().getName() );
                    System.err.println("for value: " + value.toString() );
                }
            }
            
        }
  
    }
    
    /**
     * Aggregate records into files by day and record type.
     */
    public static class SplitFilesReducer extends Reducer<Text,Text,NullWritable,Text> {
        
        private static final Logger log = Logger.getLogger(SplitFilesReducer.class);
        
        public static final int RECORD_TYPE = 1;
        public static final int RECORD_DATE = 0;
        
        MultipleOutputs<NullWritable,Text> mos;
        
        @Override
        protected void setup(Context context) throws IOException, InterruptedException {
            super.setup(context);            
            mos = new MultipleOutputs<NullWritable,Text>(context);
        }
        
       
        @Override
        protected void reduce(Text key, Iterable<Text> values, Context ctx) throws IOException, InterruptedException {
                     
            String[] components = key.toString().split("-",2);
                     
            String year = components[RECORD_DATE].substring(0,4);
            String month = components[RECORD_DATE].substring(4,6);
            
            StringBuilder outfile = new StringBuilder();
            outfile.append("/SummerCamp2015/nxcore/split/")
                   .append( components[RECORD_TYPE] )  
                   .append("/")
                   .append( year )
                   .append("/")
                   .append( month )
                   .append("/")
                   .append( components[RECORD_DATE] );  //date
            
            for ( Text value : values ){
                mos.write( NullWritable.get(), value, outfile.toString() );
            }
            
        }

        @Override
        protected void cleanup(Context context) throws IOException, InterruptedException {
            super.cleanup(context); 
            mos.close();
        }
       
    }
    
    /**
     * Set the MapReduce job configuration.
     * 
     * @param args
     * @throws Exception 
     */
    public static void main( String[] args ) throws Exception {
        
        Configuration conf = new Configuration();
        conf.setBoolean("mapreduce.map.output.compress", true);
        conf.setClass("mapreduce.map.output.compress.codec", SnappyCodec.class, CompressionCodec.class);
        
        Job job = Job.getInstance(conf, SplitFiles.NAME);
        job.setJarByClass(SplitFiles.class);
                
        job.setInputFormatClass(TextInputFormat.class);        
        job.setMapperClass(SplitFiles.SplitFilesMapper.class);
        job.setMapOutputKeyClass(Text.class);
        job.setMapOutputValueClass(Text.class);
        
        job.setReducerClass(SplitFiles.SplitFilesReducer.class);
        job.setOutputKeyClass(NullWritable.class);
        job.setOutputValueClass(Text.class);        
        //job.setOutputFormatClass(TextOutputFormat.class);
            
            //Ensure results are rolled up into one file per day.
        LazyOutputFormat.setOutputFormatClass(job, TextOutputFormat.class);
        
        job.setNumReduceTasks(26*4);
        
        TextInputFormat.addInputPath(job, new Path( args[0] ));        
        TextOutputFormat.setOutputPath(job, new Path( args[1] ));        
        TextOutputFormat.setCompressOutput(job, true);
        TextOutputFormat.setOutputCompressorClass(job, BZip2Codec.class );
                        
        System.exit(job.waitForCompletion(true) ? 0 : 1);
        
    }
    
}
