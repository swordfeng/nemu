void hs_init_with_rtsopts(int *, char **[]);
void hs_exit();
void init_monitor(int, char *[]);
void reg_test();
void restart();
void ui_mainloop();

int main(int argc, char *argv[]) {

    /* Initialize Haskell Runtime */
    hs_init_with_rtsopts(&argc, &argv);

    /* Initialize the monitor. */
    init_monitor(argc, argv);

    /* Test the implementation of the ``CPU_state'' structure. */
    reg_test();

    /* Initialize the virtual computer system. */
    restart();

    /* Receive commands from user. */
    ui_mainloop();

    /* Exit Haskell Runtime */
    hs_exit();

    return 0;
}
