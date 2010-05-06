# Copyright 1999-2009 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header: $

EAPI=2
inherit eutils qt4

DESCRIPTION="STM and STS analysis, extensible through modules"
HOMEPAGE="http://novembre.sourceforge.net"
SRC_URI="mirror://sourceforge/novembre/${P}.tar.bz2"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE="+log +verbose-log debug"

CDEPEND=">=x11-libs/qwt-5
	 x11-libs/qt-gui:4"

DEPEND="${CDEPEND}"

RDEPEND="${CDEPEND}"

src_unpack() {
	unpack ${A}
	mv 0.0.4h/* .
	rm -R 0.0.4h
}

src_prepare() {
	cat > Novembre.pri <<-EOF
	NVB_BIN_INSTALL_PATH = /usr/bin
	NVB_PLUGIN_INSTALL_PATH = /usr/lib/Novembre
	INCLUDEPATH += /usr/include/qwt5
	LIBS += -lqwt
	EOF

	if use debug; then
		cat >> Novembre.pri <<-EOF
		CONFIG += debug NVBLog NVBVerboseLog
		DEFINES += NVB_DEBUG
		EOF
	elif use log; then
		echo "CONFIG += NVBLog" >> Novembre.pri
		if use verbose-log; then
			echo "CONFIG += NVBVerboseLog" >> Novembre.pri
		fi
	fi

	cat Novembre.pri
}

src_configure() {
	eqmake4
}

src_compile() {
	emake || die "emake failed"
}

src_install() {
	emake INSTALL_ROOT="${D}" install || die 'emake install failed'
	newicon icons/novembre_v0.xpm novembre.xpm
	make_desktop_entry novembre "Novembre" novembre.xpm
}
