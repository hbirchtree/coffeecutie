switch ($args[0]) {
    "quick-build" {
        . $PWD\toolchain\quick-build.ps1 $args[1..($args.length-1)]
    }
    default {
        echo `
"Available tools: `
quick-build   - Setup build using pre-defined environments"
    }
}
