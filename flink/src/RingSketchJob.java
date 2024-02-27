package ringsketch;

import org.apache.flink.api.common.typeinfo.Types;
import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.api.java.utils.ParameterTool;
import org.apache.flink.streaming.api.datastream.DataStream;
import org.apache.flink.streaming.api.environment.StreamExecutionEnvironment;

public class RingSketchJob {
    public static void main(String[] args) throws Exception {
        ParameterTool params = ParameterTool.fromArgs(args);

        StreamExecutionEnvironment env = StreamExecutionEnvironment.getExecutionEnvironment();

        DataStream<Tuple2<Long, Long>> source = env
                .readTextFile(params.get("input"))
                .map(x -> {
                    String[] value = x.split(",");
                    return new Tuple2<>(Long.parseUnsignedLong(value[0]), Long.parseUnsignedLong(value[1]));
                })
                .returns(Types.TUPLE(Types.LONG, Types.LONG));

        DataStream<Integer> keyedStream = source
                .keyBy(value -> value.f0)
                .process(new RingSketchFunction())
                .name("ring-sketch-function");

        keyedStream.print();

        env.execute("Ring Sketch Insertion");
    }
}
