/*
	PortAL - GUIPro Project ( http://obsidev.github.io/guipro/ )

	Author : Glatigny J�r�me <jerome@obsidev.com> - http://www.obsidev.com/

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "common.h"
#include "xmlconfig.h"
#include "config.h"
#include "hotKey.h"
#include "pugixml/pugixml.hpp"

UINT defaultShowShortcut = 0x0;

/* ------------------------------------------------------------------------------------------------- */

UINT isTrue(const wchar_t* cTemp, UINT ret = 0x0)
{
	if( (cTemp != NULL) && (cTemp[0] != L'\0') )
	{
		if( !_wcsicmp(cTemp, L"true") )
			return 0x1;
		if( !_wcsicmp(cTemp, L"yes") )
			return 0x1;
		if( ret == 0x1 ) {
			if( !_wcsicmp(cTemp, L"false") )
				return 0x0;
			if( !_wcsicmp(cTemp, L"no") )
				return 0x0;
		}
	}
	return ret;
}

/* ------------------------------------------------------------------------------------------------- */

void getGeneralOptions(PortalConfig* config, pugi::xml_node elem)
{
	if( isTrue(elem.attribute(L"defaultshell").value() ) ) 
		config->shellExecuteDefault = true;
	
	defaultShowShortcut = 0x0;
	if( isTrue(elem.attribute(L"showshortcut").value() ) ) 
		defaultShowShortcut = 0x1;
}

/* ------------------------------------------------------------------------------------------------- */

UINT getOptions(pugi::xml_node elem)
{
	UINT options = PROG_OPTION_NONE;

	if( isTrue(elem.attribute(L"default").value() ) ) 
		options |= PROG_OPTION_DEFAULT;

	const wchar_t* cTemp = elem.attribute(L"shell").value();
	if( (cTemp != NULL) && (cTemp[0] != L'\0') )
	{
		if( isTrue(elem.attribute(L"shell").value() ) ) 
		{
			options |= PROG_OPTION_SHELL;
		}
		else
		{
			options |= PROG_OPTION_NOSHELL;
		}
	}

	if( isTrue(elem.attribute(L"autorun").value()) ) 
		options |= PROG_OPTION_AUTORUN;

	if( isTrue(elem.attribute(L"browse").value()) )
		options |= PROG_OPTION_BROWSE | PROG_OPTION_SHELL;

	if( isTrue(elem.attribute(L"showsc").value(), defaultShowShortcut) )
		options |= PROG_OPTION_SHOWSC;

	if( isTrue(elem.attribute(L"break").value()) )
		options |= PROG_OPTION_BREAK;

	if (isTrue(elem.attribute(L"elevate").value()))
		options |= PROG_OPTION_ELEVATION;

	return options;
}

/* ------------------------------------------------------------------------------------------------- */

void loadSubMenu(pugi::xml_node elem, PortalProg* container, PortalConfig* config)
{
	PortalProg* l_prog = NULL;
	PortalProg* prog_tmp = NULL;
	wchar_t* keyX = _wcsdup(L"key0");
	wchar_t* modX = _wcsdup(L"mod0");
	wchar_t* overX = _wcsdup(L"over0");

	while( elem )
	{
		keyX[3] = '1';
		modX[3] = '1';
		overX[4] = '1';

		if( !wcscmp(elem.name(), L"app" ) )
		{
			// Add app in the menu
			//
			l_prog = new PortalProg();

			if(!elem.attribute(L"name").empty())
				l_prog->progName = _wcsdup(elem.attribute(L"name").value());

			if(!elem.attribute(L"key").empty()) {
				l_prog->hkey = getHotKeyCode(elem.attribute(L"key").value());
				l_prog->modifier = MOD_WIN;
			}

			if(!elem.attribute(L"mod").empty())
				l_prog->modifier = getModifier(elem.attribute(L"mod").value());

			if(!elem.attribute(L"over").empty())
				l_prog->overriding = (isTrue(elem.attribute(L"over").value()) != 0x0);

			if(!elem.attribute(L"ico").empty())
				l_prog->icoPath = specialDirs(elem.attribute(L"ico").value());

			if(!elem.attribute(L"path").empty())
				l_prog->dirPath = specialDirs(elem.attribute(L"path").value());

			if(!elem.attribute(L"exe").empty())
				l_prog->progExe = specialDirs(elem.attribute(L"exe").value());

			if(!elem.attribute(L"param").empty())
				l_prog->progParam = specialDirs(elem.attribute(L"param").value());
			l_prog->options = getOptions(elem);

			container->progs.push_back( l_prog );

			pushHotkey(config, l_prog);
			config->flat.push_back( l_prog );
			l_prog->uID = (UINT)config->flat.size();
		}
		else if( !wcscmp(elem.name(), L"group" ) )
		{
			l_prog = new PortalProg();

			if(!elem.attribute(L"name").empty())
				l_prog->progName = _wcsdup(elem.attribute(L"name").value());

			if(!elem.attribute(L"key").empty()) {
				l_prog->hkey = getHotKeyCode(elem.attribute(L"key").value());
				l_prog->modifier = MOD_WIN;
			}

			if(!elem.attribute(L"mod").empty())
				l_prog->modifier = getModifier(elem.attribute(L"mod").value());

			if(!elem.attribute(L"over").empty())
				l_prog->overriding = (isTrue(elem.attribute(L"over").value()) != 0x0);

			if(!elem.attribute(L"ico").empty())
				l_prog->icoPath = specialDirs(elem.attribute(L"ico").value());

			l_prog->options = getOptions(elem);

			container->progs.push_back( l_prog );

			pushHotkey(config, l_prog);
			config->flat.push_back( l_prog );
			l_prog->uID = (UINT)config->flat.size();

			loadSubMenu(elem.first_child(), l_prog, config );
		}
		else
		{
			// Add extra entry in the menu
			//
			l_prog = new PortalProg();

			if(!elem.attribute(L"name").empty())
				l_prog->progName = _wcsdup(elem.attribute(L"name").value());

			if(!elem.attribute(L"key").empty()) {
				l_prog->hkey = getHotKeyCode(elem.attribute(L"key").value());
				l_prog->modifier = MOD_WIN;
			}

			if(!elem.attribute(L"mod").empty())
				l_prog->modifier = getModifier(elem.attribute(L"mod").value());

			if(!elem.attribute(L"over").empty())
				l_prog->overriding = (isTrue(elem.attribute(L"over").value()) != 0x0);

			if(!elem.attribute(L"ico").empty())
				l_prog->icoPath = specialDirs(elem.attribute(L"ico").value());

			l_prog->options = getOptions(elem);

			if( !wcscmp(elem.name(), L"sep" ) )
			{
				l_prog->progExe = _wcsdup(L"|sep");
			}
			else if( !wcscmp(elem.name(), L"quit" ) )
			{
				l_prog->progExe = _wcsdup(L"|quit");
			}
			else if( !wcscmp(elem.name(), L"about" ) )
			{
				l_prog->progExe = _wcsdup(L"|about");
			}

			container->progs.push_back( l_prog );

			pushHotkey(config, l_prog);
			config->flat.push_back( l_prog );
			l_prog->uID = (UINT)config->flat.size();
		}

		while(!elem.attribute(keyX).empty())
		{
			prog_tmp = new PortalProg();
			prog_tmp->hkey = getHotKeyCode(elem.attribute(keyX).value());
			prog_tmp->modifier = getModifier(elem.attribute(modX).value());
			prog_tmp->overriding = (isTrue(elem.attribute(overX).value()) != 0x0);
			prog_tmp->nextSameHotkey = l_prog;
			pushHotkey(config, prog_tmp);
			if( keyX[3] < L'9' )
			{
				keyX[3] = keyX[3]+1;
				modX[3] = modX[3]+1;
				overX[3] = overX[4]+1;
			}
		}

		elem = elem.next_sibling();
	}
	free(keyX);
	free(modX);
	free(overX);
}

/* ------------------------------------------------------------------------------------------------- */

void loadVariables(pugi::xml_node elem, PortalVariableVector* variables)
{
	PortalVariable* var = NULL;

	for (PortalVariableVector::iterator i = variables->begin(); i != variables->end(); i++)
	{
		free((*i)->key);
		free((*i)->value);
		free((*i));
	}
	variables->clear();

	while (elem)
	{
		if (wcscmp(elem.name(), L"var") || elem.attribute(L"name").empty()) {
			elem = elem.next_sibling();
			continue;
		}

		var = (PortalVariable*)malloc(sizeof(PortalVariable));
		var->key = _wcsdup(elem.attribute(L"name").value());
		var->value = NULL;

		if (!elem.attribute(L"value").empty())
			var->value = specialDirs(elem.attribute(L"value").value());
		
		variables->push_back(var);
		elem = elem.next_sibling();
	}
}

/* ------------------------------------------------------------------------------------------------- */

void pushHotkey(PortalConfig* config, PortalProg* l_prog)
{
	if (l_prog->hkey <= 0)
		return;
	
	for( PortalProgVector::iterator i = config->hotkeys.begin() ; i != config->hotkeys.end() ; i++ )
	{
		if( ((*i)->hkey == l_prog->hkey) && ((*i)->modifier == l_prog->modifier) )
		{
			(*i)->nextSameHotkey = l_prog;
			return;
		}
	}
	config->hotkeys.push_back( l_prog );
}

/* ------------------------------------------------------------------------------------------------- */

PortalConfig* loadConfig(wchar_t* filename)
{
	if(filename == NULL)
		return NULL;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(filename);

	if( !result )
		return NULL;

	PortalConfig* config = new PortalConfig();
	PortalProg* l_prog = NULL;
	PortalProg* prog_tmp = NULL;
	wchar_t* keyX = _wcsdup(L"key0");
	wchar_t* modX = _wcsdup(L"mod0");
	wchar_t* overX = _wcsdup(L"over0");

	pugi::xml_node root = doc.first_child();
	getGeneralOptions(config, root);

	pugi::xml_node menu = root.first_child();
	while( menu )
	{
		if (!wcscmp(menu.name(), L"app") || !wcscmp(menu.name(), L"group"))
		{
			l_prog = new PortalProg();
			keyX[3] = L'1';
			modX[3] = L'1';
			overX[4] = L'1';

			if (!wcscmp(menu.name(), L"app"))
			{
				if (!menu.attribute(L"name").empty())
					l_prog->progName = _wcsdup(menu.attribute(L"name").value());
				if (!menu.attribute(L"key").empty()) {
					l_prog->hkey = getHotKeyCode(menu.attribute(L"key").value());
					l_prog->modifier = MOD_WIN;
				}
				if (!menu.attribute(L"mod").empty())
					l_prog->modifier = getModifier(menu.attribute(L"mod").value());
				if (!menu.attribute(L"over").empty())
					l_prog->overriding = (isTrue(menu.attribute(L"over").value()) != 0x0);
				if (!menu.attribute(L"ico").empty())
					l_prog->icoPath = specialDirs(menu.attribute(L"ico").value());
				if (!menu.attribute(L"path").empty())
					l_prog->dirPath = specialDirs(menu.attribute(L"path").value());
				if (!menu.attribute(L"exe").empty())
					l_prog->progExe = specialDirs(menu.attribute(L"exe").value());
				if (!menu.attribute(L"param").empty())
					l_prog->progParam = specialDirs(menu.attribute(L"param").value());
				l_prog->options = getOptions(menu);
			}
			else if (!wcscmp(menu.name(), L"group"))
			{
				if (!menu.attribute(L"name").empty())
					l_prog->progName = _wcsdup(menu.attribute(L"name").value());
				if (!menu.attribute(L"key").empty()) {
					l_prog->hkey = getHotKeyCode(menu.attribute(L"key").value());
					l_prog->modifier = MOD_WIN;
				}
				if (!menu.attribute(L"mod").empty())
					l_prog->modifier = getModifier(menu.attribute(L"mod").value());
				if (!menu.attribute(L"over").empty())
					l_prog->overriding = (isTrue(menu.attribute(L"over").value()) != 0x0);
				if (!menu.attribute(L"ico").empty())
					l_prog->icoPath = specialDirs(menu.attribute(L"ico").value());
				l_prog->options = getOptions(menu);

				if (isTrue(menu.attribute(L"autoopen").value()) == 0x1)
					l_prog->options |= PROG_OPTION_AUTOOPEN;
			}

			pushHotkey(config, l_prog);
			config->flat.push_back(l_prog);
			l_prog->uID = (UINT)config->flat.size();

			while (!menu.attribute(keyX).empty())
			{
				prog_tmp = new PortalProg();
				prog_tmp->uID = (UINT)(-1); // Set special uID for memory allocation.
				prog_tmp->hkey = getHotKeyCode(menu.attribute(keyX).value());
				prog_tmp->modifier = getModifier(menu.attribute(modX).value());
				prog_tmp->overriding = (isTrue(menu.attribute(overX).value()) != 0x0);
				prog_tmp->nextSameHotkey = l_prog;
				pushHotkey(config, prog_tmp);
				if (keyX[3] < L'9')
				{
					keyX[3] = keyX[3] + 1;
					modX[3] = modX[3] + 1;
					overX[4] = overX[4] + 1;
				}
			}

			loadSubMenu(menu.first_child(), l_prog, config);

			config->menus.push_back(l_prog);
		}
		else if (!wcscmp(menu.name(), L"variables"))
		{
			if (!g_variables)
				g_variables = new PortalVariableVector();
			loadVariables(menu.first_child(), g_variables);
		}

		menu = menu.next_sibling();
	}
	free(keyX);
	free(modX);
	free(overX);

	return config;
}

/* ------------------------------------------------------------------------------------------------- */