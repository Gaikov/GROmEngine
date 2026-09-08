Module["preRun"] = Module["preRun"] || [];
Module["preRun"].push(() => {
    const mountPath = "/internal";
    const dependency = "idbfs-internal-load";
    let completed = false;
    let persistentMounted = false;

    const finish = () => {
        if (!completed) {
            completed = true;
            removeRunDependency(dependency);
        }
    };

    const useTemporaryStorage = (error) => {
        console.warn(
            "Persistent internal storage is unavailable; using temporary storage for this session.",
            error
        );

        if (persistentMounted) {
            try {
                FS.unmount(mountPath);
                persistentMounted = false;
            } catch (unmountError) {
                console.warn("Failed to unmount IDBFS internal storage.", unmountError);
            }
        }

        try {
            FS.mount(MEMFS, {}, mountPath);
        } catch (mountError) {
            console.error("Failed to mount temporary internal storage.", mountError);
        }

        finish();
    };

    addRunDependency(dependency);

    try {
        FS.mkdirTree(mountPath);
        FS.mount(IDBFS, { autoPersist: true }, mountPath);
        persistentMounted = true;
        FS.syncfs(true, (error) => {
            if (error) {
                useTemporaryStorage(error);
                return;
            }

            finish();
        });
    } catch (error) {
        useTemporaryStorage(error);
    }
});
