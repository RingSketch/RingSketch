package ringsketch;

import java.util.ArrayList;
import java.util.List;

import static java.lang.Integer.*;

public class RingSketch implements java.io.Serializable{
    private final String NAME;
    private final int WIDTH;
    private final int UPDATE_SPEED;
    private final int COUNTER_SIZE;
    private final int COUNTER_NUM;
    private final int COUNTER_INTERVAL;
    private final int[] Clocks;

    private static final CustomHashFunctions HASH = new CustomHashFunctions("SHA-256");
    private int lastUpdateIdx;

    public RingSketch(int window, int memory, int counterNum, int counterSize) {
        NAME = "RingSketch";
        WIDTH = 8 * memory / counterSize;
        COUNTER_INTERVAL = WIDTH / counterNum;
        UPDATE_SPEED = (int) ((1L << counterSize) * WIDTH / window);

        COUNTER_NUM = counterNum;
        COUNTER_SIZE = counterSize;

        Clocks = new int[WIDTH];
        lastUpdateIdx = 0;
    }

    public void insert(long x){
        int pos;
        for (int i = 0; i < COUNTER_NUM; i++) {
            pos = i * COUNTER_INTERVAL + Integer.remainderUnsigned(HASH.hashToInt(x, 200 + i), COUNTER_INTERVAL) + WIDTH;
            pos = pos % WIDTH;

            Clocks[pos] = (1 << COUNTER_SIZE) - 1;
        }
    }

    public void updateClock(int insertTimesPerUpdate) {
        int temp = UPDATE_SPEED * insertTimesPerUpdate;
        int subAll = temp / WIDTH;
        int len = temp % WIDTH;
        updateRange(0, WIDTH, subAll);

        int beg, end;
        if (lastUpdateIdx + len >= WIDTH) {
            beg = lastUpdateIdx;
            end = (lastUpdateIdx + len) % WIDTH;
            updateRange(beg, WIDTH, 1);
            updateRange(0, end, 1);
        }else {
            beg = lastUpdateIdx;
            end = lastUpdateIdx + len;
            updateRange(beg, end, 1);
        }
        lastUpdateIdx = end;
    }

    private void updateRange(int beg, int end, int val) {
        if (val <= 0)
            return;

        for (int j = beg; j < end; j++)
            Clocks[j] = max(0, Clocks[j] - val);
    }

    private int distance(int a, int b) {
        if (a <= b)
            return b - a;
        return b - a + WIDTH;
    }

    private int median(int a, int b) {
        int d = distance(a, b);
        return (a + d / 2) % WIDTH;
    }

    public int estimate(long x)
    {
        int Min = (1 << COUNTER_SIZE) - 2;

        List<Integer> pos_array = new ArrayList<>();

        int pos;
        for (int i = 0; i < COUNTER_NUM; i++) {
            pos = i * COUNTER_INTERVAL + Integer.remainderUnsigned(HASH.hashToInt(x, 200 + i), COUNTER_INTERVAL) + WIDTH;
            pos = pos % WIDTH;

            Min = min(Min, Clocks[pos]);
            pos_array.add(pos);
        }

        int pos1 = -1;
        int max_distance = 0;
        for (int i = 0; i < COUNTER_NUM; i++) {
            pos = pos_array.get(i);

            if (Clocks[pos] == Min) {
                int d = distance(pos, lastUpdateIdx);
                if (d > max_distance) {
                    max_distance = d;
                    pos1 = pos;
                }
            }
        }

        int pos2 = -1;
        int min_distance = WIDTH;
        for (int i = 0; i < COUNTER_NUM; i++) {
            pos = pos_array.get(i);

            if (Clocks[pos] == Min + 1) {
                int d = distance(pos, lastUpdateIdx);
                if (d > max_distance && d < min_distance) {
                    min_distance = d;
                    pos2 = pos;
                }
            }
        }

        if (pos2 == -1)
            min_distance = max_distance = WIDTH;

        return (int) ((WIDTH * ((1L << COUNTER_SIZE) - 2 - Min) + (min_distance + max_distance) / 2) / UPDATE_SPEED);
    }
}
