package mil.darpa.nxcore;

import java.util.ArrayList;
import java.util.List;
import org.apache.hadoop.io.LongWritable;
import org.apache.hadoop.io.NullWritable;
import org.apache.hadoop.io.Text;
import org.apache.hadoop.mrunit.mapreduce.MapDriver;
import org.apache.hadoop.mrunit.mapreduce.MapReduceDriver;
import org.apache.hadoop.mrunit.mapreduce.ReduceDriver;
import org.junit.Before;
import org.junit.Test;

/**
 *
 * @author Matt Parker
 */
public class SplitFilesTest {
    
    MapDriver<LongWritable, Text, Text, Text> mapDriver;
    ReduceDriver<Text, Text, NullWritable, Text>reduceDriver;
    MapReduceDriver<LongWritable, Text, Text, Text, NullWritable, Text> mapreduceDriver;
    
    @Before
    public void setUp(){
        
        SplitFiles.SplitFilesMapper mapper = new SplitFiles.SplitFilesMapper();
        SplitFiles.SplitFilesReducer reducer = new SplitFiles.SplitFilesReducer();
        
        mapDriver = MapDriver.newMapDriver(mapper);
        reduceDriver = ReduceDriver.newReduceDriver(reducer);
        mapreduceDriver = MapReduceDriver.newMapReduceDriver(mapper,reducer);
        
    }
    
    //Test
    public void testMapper() throws Exception {
        mapDriver.withInput( new LongWritable(1), new Text() );
        mapDriver.withOutput( new Text(), new Text() );
        mapDriver.runTest();
    }
    
    //Test
    public void testReducer() throws Exception {
        
        List<Text> values = new ArrayList<Text>();
        values.add( new Text() );
        
        reduceDriver.withInput( new Text(), values );
        reduceDriver.withOutput( NullWritable.get(), new Text() );
        reduceDriver.runTest();
    }
    
    //Test
    public void testMapReduce(){
        
        
    }
    
}
