package ringsketch;

import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

public class CustomHashFunctions {

    private final String algorithm;
    private final SecureRandom random;

    public CustomHashFunctions(String algorithm) {
        this.algorithm = algorithm;

        // Use a secure random number generator for seeding
        this.random = new SecureRandom();
    }

    public byte[] hash(byte[] data, long seed) {
        try {
            // Create a MessageDigest instance with the specified algorithm
            MessageDigest digest = MessageDigest.getInstance(algorithm);

            // Add the seed to the data
            byte[] seedBytes = longToBytes(seed);
            byte[] dataWithSeed = new byte[data.length + seedBytes.length];
            System.arraycopy(seedBytes, 0, dataWithSeed, 0, seedBytes.length);
            System.arraycopy(data, 0, dataWithSeed, seedBytes.length, data.length);

            // Update the digest with the combined data and seed
            digest.update(dataWithSeed);

            // Calculate the hash and return it
            return digest.digest();
        } catch (NoSuchAlgorithmException e) {
            // Handle the exception or throw it further
            throw new RuntimeException("Hash algorithm not found", e);
        }
    }

    private byte[] longToBytes(long value) {
        byte[] result = new byte[8];
        for (int i = 7; i >= 0; i--) {
            result[i] = (byte) (value & 0xFF);
            value >>= 8;
        }
        return result;
    }

    private static String bytesToHex(byte[] bytes) {
        StringBuilder result = new StringBuilder();
        for (byte b : bytes) {
            result.append(String.format("%02X", b));
        }
        return result.toString();
    }

    public int hashToInt(long data, long seed) {
        byte[] hashBytes = hash(longToBytes(data), seed);

        // Take the first 4 bytes of the hash and convert them to an integer
        int result = 0;
        for (int i = 0; i < 4; i++) {
            result |= (hashBytes[i] & 0xFF) << (8 * (3 - i));
        }

        return result;
    }
}
