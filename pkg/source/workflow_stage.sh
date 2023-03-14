#!/bin/sh

# Propagate failure.
set -e

source_root="$1"
final_dst="$2"

version=$(grep -m1 -E -e 'version = ' "${source_root}/MODULE.bazel" | sed -E -e 's/.*"(.*)".*/\1/')

mkdir -p "${final_dst}"
cd "${source_root}"

git archive \
  --format=tar.gz \
  -o "${final_dst}/rules_sxproto-${version}.tar.gz" \
  --prefix "rules_sxproto-${version}/" \
  HEAD

