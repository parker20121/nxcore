package mil.darpa.nxcore;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.io.compress.BZip2Codec;
import org.apache.hadoop.mapreduce.Job;
import org.apache.hadoop.mapreduce.Mapper;
import org.apache.hadoop.mapreduce.Reducer;
import org.apache.hadoop.mapreduce.lib.input.FileInputFormat;
import org.apache.hadoop.mapreduce.lib.input.FileSplit;
import org.apache.hadoop.mapreduce.lib.output.FileOutputFormat;
import org.apache.hadoop.mapreduce.lib.output.MultipleOutputs;
import org.apache.hadoop.mapreduce.lib.output.TextOutputFormat;
import org.apache.hadoop.util.GenericOptionsParser;


/**
 * MapReduce class used to split the NXCore Windows output running on Linux.
 * 
 * @author Matt Parker (matthew.parker@l-3com.com)
 * 
 */
public class SplitFiles {
    
    /**
     * Spilt the file data into the four separate data types.
     */
    public static class SplitFilesMapper extends Mapper<LongWritable, Text, Text, Text>{

        public static final int RECORD_TYPE = 0;
        public static final int RECORD = 1;
        
        @Override
        protected void map( LongWritable key, Text value, Context context ) throws IOException, InterruptedException {  
            
                //Go from 20140101.XA.nvc.txt.bz2 -> 20140101
            String filekey = ((FileSplit) context.getInputSplit()).getPath().getName().substring(0,8);           
            
                //Split record type, record
            String[] components = value.toString().split(",",1);     
            
                //Segment data by file. 
            Text filesplit = new Text( filekey + "-" + components[RECORD_TYPE] );            
            context.write( filesplit, new Text(components[RECORD]) );    
            
        }
  
    }
    
    /**
     * Aggregate records into files by day and record type.
     */
    public static class SplitFilesReducer extends Reducer<Text,Text,NullWritable,Text> {
        
        MultipleOutputs<NullWritable,Text> mos;
        
        @Override
        protected void setup(Context context) throws IOException, InterruptedException {
            super.setup(context);            
            mos = new MultipleOutputs<NullWritable,Text>(context);
        }
        
       
        @Override
        protected void reduce(Text key, Iterable<Text> values, Context context) throws IOException, InterruptedException {
         
            String[] components = key.toString().split("-",1);
            String year = components[0].substring(0,4);
            String month = components[0].substring(4,6);
            
            StringBuffer outfile = new StringBuffer();
            outfile.append("/")
                   .append( components[1] )
                   .append("/")
                   .append( year )
                   .append("/")
                   .append( month )
                   .append("/")
                   .append( components[0 ]);
            
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
        GenericOptionsParser optionParser = new GenericOptionsParser(conf, args);
        
        String[] remainingArgs = optionParser.getRemainingArgs();
        if (!(remainingArgs.length != 2)) {
          System.err.println("Usage: SplitFiles <in> <out>");
          System.exit(2);
        }
    
        Job job = Job.getInstance(conf,"split-nxcore-files");
        job.setJarByClass(SplitFiles.class);
        job.setMapperClass(SplitFiles.SplitFilesMapper.class);
        job.setReducerClass(SplitFiles.SplitFilesReducer.class);
        job.setOutputKeyClass(LongWritable.class);
        job.setOutputValueClass(Text.class);
        
        List<String> otherArgs = new ArrayList<String>();
        for (int i=0; i < remainingArgs.length; ++i) {
          otherArgs.add(remainingArgs[i]);
        }
    
        FileInputFormat.addInputPath(job, new Path( otherArgs.get(0) ));
        FileOutputFormat.setOutputPath(job, new Path( otherArgs.get(1) ));
        
        TextOutputFormat.setCompressOutput(job, true);
        TextOutputFormat.setOutputCompressorClass(job, BZip2Codec.class );
        
        System.exit(job.waitForCompletion(true) ? 0 : 1);
        
    }
    
}
