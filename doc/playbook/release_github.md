# Releasing a new version on GitHub

## Variables
```shell
RELEASE_TAG=v1.2.3
```

## Procedure
Remember to set [environment variables](#variables) for the commands below.

First update the version number in `MODULE.bazel` and commit that change.

Then tag the commit and push it.
```shell
git tag "${RELEASE_TAG}"
git push origin "${RELEASE_TAG}"
```

Simply pushing the tag will induce a release workflow.
Just fill in the release info.

Last, add the new version to [fildesh_bazel_registry](https://github.com/fildesh/fildesh_bazel_registry) and try using it in a dependent project like [rendezqueue](https://github.com/rendezqueue/rendezqueue).

### Reverting
If the release step screwed up, just delete the tag remotely and locally.
```shell
git push --delete origin "${RELEASE_TAG}"
git tag -d "${RELEASE_TAG}"
```

Assuming a release was made, you'll also want to delete that on GitHub.
