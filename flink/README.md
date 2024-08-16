# RingSketch on Apache Flink

We implement RingSketch on top of Apache Flink to show that our solution can be easily integrated into modern stream processing framework and work in distributed environment. The results show that RingSketch can smoothly work on top of Flink framework.

## File description

`./src/RingSketch.java`: The implementation of the basic RingSketch.
`./src/RingSketchFunction.java`: A wrapper class which wraps the original RingSketch.
`./src/RingSketchJob.java`: The implementation of the main method, which gets its input stream by reading the (HDFS) file, and insert them into the RingSketch.
`./src/CustomHashFunctions.java`: A set of pairwise-independent hash functions utilizing existing hash algorithms (SHA-256 in our implementation of RingSketch).
`flink.dat`: The sample input file, which is generated based on Criteo datasets.

## Input file

We generate the input file based on Criteo datasets. We present a sample dataset `flink.dat` here, which consists of 1,048,576 items, among which 195,084 items are distinct.

The format of the input file is as follows. Each row of the input file is a 2-tuple consisting of an ID (Long) and a timestamp (Long), seperated by a comma.

```
id1,timestamp1
id2,timestamp2
id3,timestamp3
......
```

## Requirements

- `Flink 1.18.0`
- `Hadoop 2.10.2`

To address the problem of dependency when using Hadoop Distributed File System in Flink, [flink-shaded-hadoop-2-uber-2.8.3-10.0.jar]([Central Repository: org/apache/flink/flink-shaded-hadoop-2-uber/2.8.3-10.0](https://repo1.maven.org/maven2/org/apache/flink/flink-shaded-hadoop-2-uber/2.8.3-10.0/)) is needed. Please add it to `{FLINK_HOME}/lib/`.

## How to run

1. Build the package. The jar package can be built by `Maven 3.2.5`.
2. Set up Flink configuration and Hadoop Distributed File System. Copy the input file to HDFS.
3. Run. Suppose the path of the input file is `hdfs:///flink.dat`, and the path of the jar package is `/root/RingSketch.jar`, you can use the following command to run the codes.

```bash
$ {FLINK_HOME}/bin/start-cluster.sh
$ {FLINK_HOME}/bin/flink run /root/RingSketch.jar -p k --input hdfs:///flink.dat
```

where `k` is the parallism (# parallel instances) in Flink.

Stop the Flink cluster after the experiment by:

```bash
$ {FLINK_HOME}/bin/stop-cluster.sh
```