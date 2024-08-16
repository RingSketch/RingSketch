package ringsketch;

import org.apache.flink.api.java.tuple.Tuple2;
import org.apache.flink.streaming.api.functions.KeyedProcessFunction;
import org.apache.flink.util.Collector;

public class RingSketchFunction extends KeyedProcessFunction<Long, Tuple2<Long, Long>, Integer>  {
    RingSketch ringSketch = new RingSketch(1 << 13, 32000, 1, 16);
    @Override
    public void processElement(
            Tuple2<Long, Long> item,
            Context ctx,
            Collector<Integer> collector) {
        // process each element in the stream
        long x = item.f0;
        ringSketch.insert(x);
        ringSketch.updateClock(1);
    }
}
