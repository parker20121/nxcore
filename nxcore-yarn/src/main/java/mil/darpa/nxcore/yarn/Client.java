package mil.darpa.nxcore.yarn;

import java.io.File;
import java.io.IOException;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import java.util.zip.ZipFile;

import org.apache.hadoop.conf.Configuration;
import org.apache.hadoop.fs.FileStatus;
import org.apache.hadoop.fs.FileSystem;
import org.apache.hadoop.fs.Path;
import org.apache.hadoop.yarn.api.ApplicationConstants.Environment;
import org.apache.hadoop.yarn.api.protocolrecords.GetNewApplicationResponse;
import org.apache.hadoop.yarn.api.records.ApplicationId;
import org.apache.hadoop.yarn.api.records.ApplicationReport;
import org.apache.hadoop.yarn.api.records.ApplicationSubmissionContext;
import org.apache.hadoop.yarn.api.records.ContainerLaunchContext;
import org.apache.hadoop.yarn.api.records.LocalResource;
import org.apache.hadoop.yarn.api.records.LocalResourceType;
import org.apache.hadoop.yarn.api.records.LocalResourceVisibility;
import org.apache.hadoop.yarn.api.records.Resource;
import org.apache.hadoop.yarn.api.records.YarnApplicationState;
import org.apache.hadoop.yarn.client.api.YarnClient;
import org.apache.hadoop.yarn.client.api.YarnClientApplication;
import org.apache.hadoop.yarn.conf.YarnConfiguration;
import org.apache.hadoop.yarn.util.Apps;
import org.apache.hadoop.yarn.util.ConverterUtils;
import org.apache.hadoop.yarn.util.Records;

/**
 * 
 * 
 * @author Matt Parker
 * 
 * @see <a href="https://github.com/hortonworks/simple-yarn-app">HortonWorks Simple Yarn Application Code</a>
 * @see <a href="http://hadoop.apache.org/docs/current/hadoop-yarn/hadoop-yarn-site/WritingYarnApplications.html">Writing YARN Applications</a>
 * 
 */
public class Client {

    public static final String APPLICATION_NAME = "ncore-yarn";
    public static final String JARFILE = "nxcore-yarn-1.0-with-dependencies.jar";
    public static final String ZIPFILE = "nxcore-scripts.zip";
    public static final String DEFAULT_APPLICATION_QUEUE = "default";
    
    Configuration conf = new YarnConfiguration();

    public void run(String[] args) throws Exception {

            //Print command line.
        for ( int i=0; i<args.length; i++){
            System.out.print( args[i] + " ");
        }
        
        if (args.length != 5) {
            System.out.println("usage: java -cp " + JARFILE + " mil.darpa.nxcore.yarn.Client [jarpath] [full script path] [number of processors] [HDFS input dir] [HDFS output dir]");
            throw new Exception("Application not properly configured.");
        }

        final String script = args[1];
        final Path jarPath = new Path(args[0]);
        final ZipFile zipFile = null;
        final Path zipFilePath = new Path( args[1] );       
        final int n = Integer.valueOf(args[2]);
        final String hdfsInputDirectory = args[3];
        final String hdfsOutputDirectory = args[4];

        // Create yarnClient
        YarnConfiguration conf = new YarnConfiguration();
        YarnClient yarnClient = YarnClient.createYarnClient();
        yarnClient.init(conf);
        yarnClient.start();

        // Create application via yarnClient
        YarnClientApplication app = yarnClient.createApplication();
        GetNewApplicationResponse appResponse = app.getNewApplicationResponse();
        
        Resource max = appResponse.getMaximumResourceCapability();
        System.out.println("max available - cores: " + max.getVirtualCores() + " mem: " + max.getMemory() );
        
        // Set up the container launch context for the application master
        ContainerLaunchContext amContainer = Records.newRecord(ContainerLaunchContext.class);
        amContainer.setCommands(
                Collections.singletonList(
                        "$JAVA_HOME/bin/java"
                        + " -Xmx256M"
                        + " mil.darpa.nxcore.yarn.ApplicationMaster"
                        + " " + script 
                        + " " + String.valueOf(n)
                        + " " + hdfsInputDirectory
                        + " " + hdfsOutputDirectory
                )
        );

        // Setup jar for ApplicationMaster
        LocalResource appMasterJar = Records.newRecord(LocalResource.class);
        setupAppMasterJar(jarPath, appMasterJar);
        
        //Setup zip for ApplicationMaster
        LocalResource scriptJar = Records.newRecord(LocalResource.class);
        setupAppMasterJar( scriptJar, jarPath );
        Map<String,LocalResource> localResources = Collections.singletonMap(JARFILE, appMasterJar);
        localResources.put("", scriptJar );
        amContainer.setLocalResources( localResources );

        // Setup CLASSPATH for ApplicationMaster
        Map<String, String> appMasterEnv = new HashMap<String, String>();
        setupAppMasterEnv(appMasterEnv);
        amContainer.setEnvironment(appMasterEnv);

        // Set up resource type requirements for ApplicationMaster
        Resource capability = Records.newRecord(Resource.class);
        capability.setMemory(256);
        capability.setVirtualCores(1);

        // Finally, set-up ApplicationSubmissionContext for the application
        ApplicationSubmissionContext appContext = app.getApplicationSubmissionContext();
        appContext.setApplicationName( APPLICATION_NAME ); 
        appContext.setAMContainerSpec(amContainer);
        appContext.setResource(capability);
        appContext.setQueue( DEFAULT_APPLICATION_QUEUE );

        // Submit application
        ApplicationId appId = appContext.getApplicationId();
        System.out.println("Submitting NXCore extraction application " + appId);
        yarnClient.submitApplication(appContext);

        ApplicationReport appReport = yarnClient.getApplicationReport(appId);
        YarnApplicationState appState = appReport.getYarnApplicationState();
        while (appState != YarnApplicationState.FINISHED
                && appState != YarnApplicationState.KILLED
                && appState != YarnApplicationState.FAILED) {
            Thread.sleep(100);
            appReport = yarnClient.getApplicationReport(appId);
            appState = appReport.getYarnApplicationState();
        }

        System.out.println("Application " + appId + " finished with" + " state " + appState + " at " + appReport.getFinishTime());
        
    }

    private void setupAppMasterJar(Path jarPath, LocalResource appMasterJar) throws IOException {
        FileStatus jarStat = FileSystem.get(conf).getFileStatus(jarPath);
        appMasterJar.setResource(ConverterUtils.getYarnUrlFromPath(jarPath));
        appMasterJar.setSize(jarStat.getLen());
        appMasterJar.setTimestamp(jarStat.getModificationTime());
        appMasterJar.setType(LocalResourceType.FILE);
        appMasterJar.setVisibility(LocalResourceVisibility.PUBLIC);
    }
    
    private void setupZipFile(){
    
        
    }
    
    private void setupAppMasterEnv(Map<String, String> appMasterEnv) {
        
        for (String c : conf.getStrings( YarnConfiguration.YARN_APPLICATION_CLASSPATH, YarnConfiguration.DEFAULT_YARN_APPLICATION_CLASSPATH)) {
            Apps.addToEnvironment(appMasterEnv, Environment.CLASSPATH.name(),c.trim());
        }
        
        Apps.addToEnvironment(appMasterEnv, Environment.CLASSPATH.name(), Environment.PWD.$() + File.separator + "*");
        
    }

    public static void main(String[] args) throws Exception {
        Client c = new Client();
        c.run(args);
    }

}
