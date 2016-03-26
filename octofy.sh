#!/bin/sh -e
# Recreate blog and make sure all symlinks exist
# Create a new post: /usr/bin/rake new_post["title"]

cd $HOME/Troglobit/octopress

echo "==> Pushing blog to GitHub for backup ..."
git push

echo "==> Generating static content ..."
/usr/bin/rake generate

# Create compatibiltiy links from old site still visible in search
# engines and at the Internet Archive.
echo "==> Creating compatibility .shtml links ..."
cd public/
ln -s pimd.html      pimd.shtml
ln -s mrouted.html   mrouted.shtml

mkdir -p blog/2013/07/09/multicast-howto
ln -s ../../../../multicast-howto.html blog/2013/07/09/multicast-howto/index.html

ln -s tetris.html    tetris.shtml
ln -s snake.html     snake.shtml
ln -s editline.html  editline.shtml
cd -

echo "==> Deploying to mirror, http://troglobit.github.io ..."
/usr/bin/rake deploy

# Deploy to main site, make sure to cleanup any stale stuff on server
echo "==> Deploying to main site, http://troglobit.com ..."
rsync -va --delete --exclude=_deploy . troglobit.com:octopress/

echo "Done."
