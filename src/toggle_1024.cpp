// Function implementation
int toggle_1024(int wordToSend) {
    // Declare a static counter variable
    static int loopCounter = 0;

    // Increment the counter
    loopCounter++;

    // Check if the counter is even or odd and set wordToSend accordingly
    if (loopCounter % 2 == 0) {
        wordToSend = 1024;
    } else {
        wordToSend = 0;
    }

    // Optionally, reset the counter to avoid overflow (not necessary for small loops)
    if (loopCounter >= 1000000) { // Arbitrary large number
        loopCounter = 0;
    }

    return wordToSend;
}