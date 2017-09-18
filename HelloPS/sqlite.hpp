#pragma once

#include <sqlite3.h>
#include <exception>
#include <string>

namespace soechin
{
	class sqlite
	{
	private:
		sqlite3* m_db;
	protected:
		static int busy_handler(void* thiz, int count)
		{
			sqlite3_sleep(1);
			return 1;
		}
	public:
		sqlite()
		{
			m_db = NULL;
		}

		virtual ~sqlite()
		{
			close();
		}

		virtual sqlite3* ptr() const
		{
			return m_db;
		}

		virtual bool open(std::string path)
		{
			int ret;

			close();

			ret = sqlite3_open(path.c_str(), &m_db);
			if (ret != SQLITE_OK) return false;

			ret = sqlite3_busy_handler(m_db, busy_handler, this);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual void close()
		{
			if (m_db != NULL)
			{
				sqlite3_close(m_db);
				m_db = NULL;
			}
		}

		virtual bool exec(std::string sql)
		{
			int ret;

			ret = sqlite3_exec(m_db, sql.c_str(), NULL, NULL, NULL);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual std::string error()
		{
			const char* msg;

			msg = sqlite3_errmsg(m_db);
			if (msg == NULL) return std::string();

			return std::string(msg);
		}
	};

	class sqlite_stmt
	{
	private:
		sqlite3* m_db;
		sqlite3_stmt* m_stmt;
	public:
		sqlite_stmt()
		{
			m_db = NULL;
			m_stmt = NULL;
		}

		virtual ~sqlite_stmt()
		{
			finalize();
		}

		virtual sqlite3_stmt* ptr() const
		{
			return m_stmt;
		}

		virtual bool prepare(sqlite* db, std::string sql)
		{
			int ret;

			finalize();

			ret = sqlite3_prepare(db->ptr(), sql.c_str(), -1, &m_stmt, NULL);
			if (ret != SQLITE_OK) return false;

			m_db = db->ptr();
			return true;
		}

		virtual void finalize()
		{
			if (m_stmt != NULL)
			{
				sqlite3_finalize(m_stmt);
				m_stmt = NULL;
				m_db = NULL;
			}
		}

		virtual bool step()
		{
			int ret;

			ret = sqlite3_step(m_stmt);
			if (ret != SQLITE_ROW) return false;

			return true;
		}

		virtual bool reset()
		{
			int ret;

			ret = sqlite3_reset(m_stmt);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual bool bind(int index, int value)
		{
			int ret;

			ret = sqlite3_bind_int(m_stmt, index, value);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual bool bind(int index, double value)
		{
			int ret;

			ret = sqlite3_bind_double(m_stmt, index, value);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual bool bind(int index, std::string text)
		{
			int ret;

			ret = sqlite3_bind_text(m_stmt, index, text.c_str(), -1, SQLITE_TRANSIENT);
			if (ret != SQLITE_OK) return false;

			return true;
		}

		virtual bool bind(std::string name, int value)
		{
			int index;

			index = sqlite3_bind_parameter_index(m_stmt, name.c_str());
			if (!bind(index, value)) return false;

			return true;
		}

		virtual bool bind(std::string name, double value)
		{
			int index;

			index = sqlite3_bind_parameter_index(m_stmt, name.c_str());
			if (!bind(index, value)) return false;

			return true;
		}

		virtual bool bind(std::string name, std::string text)
		{
			int index;

			index = sqlite3_bind_parameter_index(m_stmt, name.c_str());
			if (!bind(index, text)) return false;

			return true;
		}

		virtual void column(int index, int& value)
		{
			value = sqlite3_column_int(m_stmt, index);
		}

		virtual void column(int index, double& value)
		{
			value = sqlite3_column_double(m_stmt, index);
		}

		virtual void column(int index, std::string& text)
		{
			const char* buf;

			buf = (const char*)sqlite3_column_text(m_stmt, index);
			text = (buf != NULL) ? buf : std::string();
		}

		virtual void column(std::string name, int& value)
		{
			int count, index;

			count = sqlite3_column_count(m_stmt);
			index = -1;

			for (int i = 0; i < count; i++)
			{
				if (sqlite3_column_name(m_stmt, i) == name)
				{
					index = i;
					break;
				}
			}

			column(index, value);
		}

		virtual void column(std::string name, double& value)
		{
			int count, index;

			count = sqlite3_column_count(m_stmt);
			index = -1;

			for (int i = 0; i < count; i++)
			{
				if (sqlite3_column_name(m_stmt, i) == name)
				{
					index = i;
					break;
				}
			}

			column(index, value);
		}

		virtual void column(std::string name, std::string& text)
		{
			int count, index;

			count = sqlite3_column_count(m_stmt);
			index = -1;

			for (int i = 0; i < count; i++)
			{
				if (sqlite3_column_name(m_stmt, i) == name)
				{
					index = i;
					break;
				}
			}

			column(index, text);
		}
	};
}
